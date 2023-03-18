# Web streaming example
# Source code from the official PiCamera package
# http://picamera.readthedocs.io/en/latest/recipes2.html#web-streaming

import io
import picamera
import logging
import socketserver
from threading import Condition
from http import server
from urllib.parse import urlparse, parse_qs

import smbus
import time

import curses

DEVICE_ADDRESS = 0x08

PAN_SPEED = 5
TILT_SPEED = 5

bus = smbus.SMBus(1)


def send_command(command1, command2):
    command1 = int(command1 + 90)
    command2 = int(command2 + 90)

    bus.write_i2c_block_data(DEVICE_ADDRESS, 0, [command1, command2])


PAGE="""\
<html>
<head>
<title>Raspberry Pi - Surveillance Camera</title>
<script>

var tiltAngle = 0;
var panAngle = 0;

function pan(speed) {
    panAngle += speed;
    sendPanTilt();
}

function tilt(speed) {
    tiltAngle += speed;
    sendPanTilt();
}

function sendPanTilt() {
    fetch("/move?pan="+panAngle+"&tilt="+tiltAngle);
}

</script>
</head>
<body>
<center><h1>Raspberry Pi - Surveillance Camera</h1></center>
<center><img src="stream.mjpg" width="640" height="480"></center>
<button onClick="pan(1)"/> &#8678; </button>
<button onClick="pan(-1)"/> &#8680; </button>
<button onClick="tilt(1)"/> &#8679; </button>
<button onClick="tilt(-1)"/> &#8681; </button>
</body>
</html>
"""

class StreamingOutput(object):
    def __init__(self):
        self.frame = None
        self.buffer = io.BytesIO()
        self.condition = Condition()

    def write(self, buf):
        if buf.startswith(b'\xff\xd8'):
            # New frame, copy the existing buffer's content and notify all
            # clients it's available
            self.buffer.truncate()
            with self.condition:
                self.frame = self.buffer.getvalue()
                self.condition.notify_all()
            self.buffer.seek(0)
        return self.buffer.write(buf)

class StreamingHandler(server.BaseHTTPRequestHandler):
    def do_GET(self):
        o = urlparse(self.path)
        if o.path == '/':
            self.send_response(301)
            self.send_header('Location', '/index.html')
            self.end_headers()
        elif o.path == '/index.html':
            content = PAGE.encode('utf-8')
            self.send_response(200)
            self.send_header('Content-Type', 'text/html')
            self.send_header('Content-Length', len(content))
            self.end_headers()
            self.wfile.write(content)
        elif o.path == '/stream.mjpg':
            self.send_response(200)
            self.send_header('Age', 0)
            self.send_header('Cache-Control', 'no-cache, private')
            self.send_header('Pragma', 'no-cache')
            self.send_header('Content-Type', 'multipart/x-mixed-replace; boundary=FRAME')
            self.end_headers()
            try:
                while True:
                    with output.condition:
                        output.condition.wait()
                        frame = output.frame
                    self.wfile.write(b'--FRAME\r\n')
                    self.send_header('Content-Type', 'image/jpeg')
                    self.send_header('Content-Length', len(frame))
                    self.end_headers()
                    self.wfile.write(frame)
                    self.wfile.write(b'\r\n')
            except Exception as e:
                logging.warning(
                    'Removed streaming client %s: %s',
                    self.client_address, str(e))
        elif o.path == '/move':
            q = parse_qs(o.query)
            camera_pan = 0
            camera_tilt = 0
            if 'pan' in q:
                pan = int(q['pan'][0])
                if pan >= -90 and pan <= 90:
                    camera_pan = pan
            if 'tilt' in q:
                tilt = int(q['tilt'][0])
                if tilt >= -90 and tilt <= 90:
                    camera_tilt = tilt
            send_command(camera_pan, camera_tilt)
        else:
            print(self.path)
            self.send_error(404)
            self.end_headers()

class StreamingServer(socketserver.ThreadingMixIn, server.HTTPServer):
    allow_reuse_address = True
    daemon_threads = True

with picamera.PiCamera(resolution='320x240', framerate=24) as camera:
    output = StreamingOutput()
    #Uncomment the next line to change your Pi's Camera rotation (in degrees)
    #camera.rotation = 90
    camera.start_recording(output, format='mjpeg')
    try:
        address = ('', 8000)
        server = StreamingServer(address, StreamingHandler)
        server.serve_forever()
    finally:
        camera.stop_recording()
