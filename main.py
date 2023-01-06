#Khai báo các thư viện
import os
import re
from gtts import gTTS
from serial import Serial
import speech_recognition as sr

#Khởi tạo lớp RC (ReCord) và cổng COM5 nối đến arduino
RC = sr.Recognizer()
Arduino_Sr = Serial('COM5', 9600)

def macAns(text):
    machine_answer = gTTS(text, lang= 'vi')
    machine_answer.save('machine_answer.mp3')
    os.system('machine_answer.mp3')
def quAns(txt):

    text = "Xin lỗi tôi không thể nghe rõ!"

    #Start a device:
    if txt == "bật đèn phòng khách":
        text = "đèn đã được bật"
        Arduino_Sr.write("1".encode())
    elif txt == "bật đèn phòng ngủ 1":
        text = "đèn đã được bật"
        Arduino_Sr.write("2".encode())
    elif txt == "bật đèn phòng ngủ 2":
        text = "đèn đã được bật"
        Arduino_Sr.write("3".encode())
    elif txt == "bật nóng lạnh":
        text = "bình nóng lạnh đã được bật, thiết bị sẽ tự động tắt khi nước đủ nóng"
        Arduino_Sr.write("4".encode())
    elif txt == "mở rèm cửa" or txt == "mở dèm cửa" or txt == "mở gièm cửa":
        text = "đã mở rèm"
        Arduino_Sr.write("5".encode())
    elif txt == "bật đèn phòng bếp":
        text = "đèn đã được bật"
        Arduino_Sr.write("6".encode())

    # Close a device:
    elif txt == "tắt đèn phòng khách":
        text = "đã tắt"
        Arduino_Sr.write("-1".encode())
    elif txt == "tắt đèn phòng ngủ 1":
        text = "đã tắt"
        Arduino_Sr.write("-2".encode())
    elif txt == "tắt đèn phòng ngủ 2":
        text = "đã tắt"
        Arduino_Sr.write("-3".encode())
    elif txt == "đóng rèm cửa" or txt == "đóng dèm cửa" or txt == "đóng gièm cửa":
        text = "đã đóng rèm che nắng"
        Arduino_Sr.write("-4".encode())
    elif txt == "tắt đèn phòng bếp":
        text = "đã tắt"
        Arduino_Sr.write("-6".encode())
    elif txt == "tắt điều hòa":
        text = "đã tắt điều hòa"
        Arduino_Sr.write("-234".encode())
    elif txt.find("bật điều hòa") != -1:
        try:
            temp = re.findall(r'\d+', txt)
            temp_int = int(temp[0])
            if temp_int < 18 or temp_int > 30:
                text = "Nhiệt độ ngoài tầm hoạt động của điều hòa!"
            else:
                text = "Điều hòa được bật ở mức " + str(temp_int) + " độ"
                Arduino_Sr.write(str(temp_int).encode())
        except:
            text = "Bạn muốn bật điều hòa ở mức bao nhiêu độ?"
    #Query:
    elif txt == "tình trạng phòng hiện tại":
        Arduino_Sr.write("123".encode())
        Arduino_Sr.readline()
        ArduinoRead = Arduino_Sr.readline().decode().split()
        text = "Nhiệt độ là 23 độ C, độ ẩm là " + str(ArduinoRead[1]) + " phần trăm, chất lượng không khí tốt"
        print(text)
    macAns(text)
while True:
    #Arduino_Sr.open()
    # Tạo biến text_audio để lưu văn bản nhận dạng được
    text_audio = "Tôi không nghe được, bạn vui lòng nói lại nhé!"
    # Dữ liệu âm thanh đầu vào được lấy từ Micro
    with sr.Microphone() as record:
        while text_audio == "Tôi không nghe được, bạn vui lòng nói lại nhé!":
            print("Đang nghe...")
            audio_in = RC.record(record, duration=5)
            try:
                text_audio = RC.recognize_google(audio_in, language="vi")
            except:
                text_audio = "Tôi không nghe được, bạn vui lòng nói lại nhé!"
                print("Trợ lý: " + text_audio)
        print("Bạn: " + text_audio)
    machine_ans = ""
    text_audio = text_audio.lower()
    if text_audio == "tắt kết nối":
        break
    quAns(text_audio)