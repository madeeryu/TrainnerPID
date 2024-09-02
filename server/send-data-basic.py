from flask import Flask, render_template, request, jsonify
import requests
from ESP32getIPaddress import esp32_ip_address

app = Flask(__name__)

# Replace with your ESP32 IP address
esp32_mac_address = 'A8:42:E3:48:39:70' # ESP32 MAC Address
ESP32_IP = f"http://{esp32_ip_address(esp32_mac_address)}"  # Adjust this to the correct IP address of the ESP32

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/set_speed', methods=['POST'])
def set_speed():
    # Get motor speed from the form
    motor_speed = request.form['speed']
    
    # Prepare the JSON payload
    data = {
        'speed': motor_speed
    }
    
    # Send the data to ESP32
    try:
        response = requests.post(f'{ESP32_IP}/set_speed', json=data)
        if response.status_code == 200:
            return jsonify({'status': 'success', 'message': 'Speed updated successfully!'})
        else:
            return jsonify({'status': 'error', 'message': 'Failed to update speed!'})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)})

if __name__ == '__main__':
    app.run(debug=True)
