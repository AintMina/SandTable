import RPi.GPIO as GPIO
import time, sys
 
red = 33
green = 12
blue = 32
white = 35
 
def setup():
    global r, g, b, w
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(red, GPIO.OUT)
    GPIO.setup(green, GPIO.OUT)
    GPIO.setup(blue, GPIO.OUT)
    GPIO.setup(white, GPIO.OUT)

    r = GPIO.PWM(red, 1000)
    g = GPIO.PWM(green, 1000)
    b = GPIO.PWM(blue, 1000)
    w = GPIO.PWM(white, 1000)

    r.start(0)
    g.start(0)
    b.start(0)
    w.start(0)


def setValues(R, G, B, W):
    R = int(int(R) / 2.55)
    G = int(int(G) / 2.55)
    B = int(int(B) / 2.55)
    W = int(int(W) / 2.55)

    r.ChangeDutyCycle(R)
    g.ChangeDutyCycle(G)
    b.ChangeDutyCycle(B)
    w.ChangeDutyCycle(W)

    while True:
        pass


def destroy():
    r.stop()
    g.stop()
    b.stop()
    w.stop()
    GPIO.output(red, GPIO.LOW)
    GPIO.output(green, GPIO.LOW)
    GPIO.output(blue, GPIO.LOW)
    GPIO.output(white, GPIO.LOW)
    GPIO.cleanup()
     
if  __name__ == '__main__':
    setup()

    if len(sys.argv) == 2:
        destroy()

    elif len(sys.argv) > 4:
        r_val = sys.argv[1]
        g_val = sys.argv[2]
        b_val = sys.argv[3]
        w_val = sys.argv[4]

        setValues(r_val, g_val, b_val, w_val)