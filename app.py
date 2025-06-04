from flask import Flask, request, jsonify, render_template, send_from_directory
import os, cv2, pytesseract
from datetime import datetime

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

# Cấu hình Tesseract OCR (tuỳ theo máy bạn)
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

@app.route('/')
def index():
    images = sorted([
        f for f in os.listdir(UPLOAD_FOLDER)
        if f.endswith('.jpg') and not (f.startswith('roi_') or f.startswith('bin_'))
    ], reverse=True)

    results = []

    for img_name in images:
        txt_path = os.path.join(UPLOAD_FOLDER, img_name + '.txt')
        digits_text = full_text = ''

        if os.path.exists(txt_path):
            with open(txt_path, 'r', encoding='utf-8') as f:
                lines = f.readlines()
                if len(lines) >= 2:
                    digits_text = lines[0].strip()
                    full_text = lines[1].strip()

        results.append((img_name, digits_text, full_text))

    return render_template('index.html', results=results)


@app.route('/upload', methods=['POST'])
def upload():
    image_data = request.stream.read()
    filename = datetime.now().strftime('%Y%m%d_%H%M%S') + '.jpg'
    filepath = os.path.join(UPLOAD_FOLDER, filename)

    with open(filepath, 'wb') as f:
        f.write(image_data)

    print(f"📷 Image received: {filename}")

    # ==== OCR tại đây ====
    try:
        img = cv2.imread(filepath)
        if img is None:
            return "Lỗi: Không đọc được ảnh", 400 

        img = cv2.resize(img, (640, 480))
        roi = img[210:270, 180:420]
        roi_path = os.path.join(UPLOAD_FOLDER, 'roi_' + filename)
        cv2.imwrite(roi_path, roi)

        gray_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
        gray_roi = cv2.GaussianBlur(gray_roi, (5, 5), 0)
        thresh_roi = cv2.adaptiveThreshold(gray_roi, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                           cv2.THRESH_BINARY_INV, 11, 2)
        config_digits = '--psm 6 -c tessedit_char_whitelist=0123456789'
        digits_text = pytesseract.image_to_string(thresh_roi, config=config_digits)
        cv2.imwrite(os.path.join(UPLOAD_FOLDER, 'bin_' + filename), thresh_roi)


        gray_full = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray_full = cv2.bilateralFilter(gray_full, 11, 17, 17)
        thresh_full = cv2.adaptiveThreshold(gray_full, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                            cv2.THRESH_BINARY_INV, 11, 2)
        config_full = '--psm 6'
        full_text = pytesseract.image_to_string(thresh_full, config=config_full)

        # Ghi file kết quả
        with open(filepath + '.txt', 'w', encoding='utf-8') as f:
            f.write(digits_text.strip() + '\n')
            f.write(full_text.strip())

    except Exception as e:
        print(f"❌ Lỗi xử lý OCR: {e}")

    return "OK", 200


@app.route('/uploads/<filename>')
def get_image(filename):
    return send_from_directory(UPLOAD_FOLDER, filename)


@app.route('/data', methods=['POST'])
def receive_data():
    data = request.get_json()
    print("📩 Dữ liệu nhận được từ ESP8266:", data)
    return jsonify({"message": "Đã nhận thành công!"}), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
