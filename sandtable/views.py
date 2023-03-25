from xml.dom import ValidationErr
from django.shortcuts import render, redirect
from .models import *
from .forms import *
import thr2png, os, psutil, writeSerial, signal

from django.http import HttpResponse
from django.views.decorators.csrf import csrf_exempt


# Commands
COMMAND_STOP = 0
COMMAND_ANGLE = 1
COMMAND_R = 2
COMMAND_X = 3
COMMAND_Y = 4
COMMAND_HOME = 5
COMMAND_UPDATE_ANGLE = 6
COMMAND_UPDATE_R = 7
COMMAND_M1_STEP = 8
COMMAND_M2_STEP = 9
COMMAND_MOTOR_SPEED = 10
COMMAND_LED_TRACK = 11
COMMAND_LED_SPEED = 12
COMMAND_LED_INTENSITY = 13
COMMAND_LED_SATURATION = 14
COMMAND_LED_R = 15
COMMAND_LED_G = 16
COMMAND_LED_B = 17
COMMAND_LED_W = 18
COMMAND_GET_ANGLE = 19
COMMAND_GET_R = 20
COMMAND_RESET = 21


playing = 0


def index(request):
    global playing
    queue = Queue.objects.all()
    track_name = ""
    if len(queue) > 0:
        track = queue[0].file
        track_name = track.split('/')[-1].replace(".thr", '')
    
    # Erase button
    if(request.GET.get('eraseButton')):
        pid = checkProcesses("PlayQueue.py")

        if not pid:
            print("Not running")
        else:
            os.kill(pid, signal.SIGTERM)
            
        return redirect('sandtable:index')

    # Button to start playing
    if(request.GET.get('playButton')):

        # Start playing
        pid = checkProcesses("PlayQueue.py")

        if not pid:
            os.system("/bin/python /home/pi/sand-table/PlayQueue.py &")
        else:
            os.kill(pid, signal.SIGUSR1)

        if playing == 0:
            playing = 1
        else:
            playing = 0

        return redirect('sandtable:index')
    
    # Next button
    if(request.GET.get('nextButton')):
        pid = checkProcesses("PlayQueue.py")

        if not pid:
            print("Not running")
        else:
            os.kill(pid, signal.SIGUSR2)

        print("next")
        return redirect('sandtable:index')

    context = { 'playing': playing, 'track_name': track_name  }
    return render(request, 'sandtable/index.html', context)


def tracks(request):
    tracks = Tracks.objects.order_by('uploaded_at')
    context = { 'tracks': tracks }

    return render(request, 'sandtable/tracks.html', context)


def track(request, track_id):
    track = Tracks.objects.get(id=track_id)

    # Button to delete track
    if(request.GET.get('deleteTrack')):
        track.delete()
        return redirect('sandtable:tracks')

    # Add track to queue
    elif request.method == 'POST':
        form = AddToQueueForm(request.POST)
        if form.is_valid():
            new_queue_track = form.save(commit=False)
            new_queue_track.file = track.file
            new_queue_track.track_length = track.track_length
            new_queue_track.pic = track.pic
            new_queue_track.save()

            # Start playing
            if not checkProcesses("PlayQueue.py"):
                os.system("/bin/python /home/pi/sand-table/PlayQueue.py &")
            
            return redirect('sandtable:tracks')
    
    else:
        form = UploadFileForm()

    context = { 'track': track, 'form': form }

    return render(request, 'sandtable/track.html', context)


def queue(request):
    queue = Queue.objects.all()
    context = { 'queue': queue }

    # If clearQueue button is pressed then delete all entries in queue table
    if(request.GET.get('clearQueue')):
        Queue.objects.all().delete()
        return redirect('sandtable:queue')

    return render(request, 'sandtable/queue.html', context)


def uploadTracks(request):
    if request.method == 'POST':
        form = UploadFileForm(request.POST, request.FILES)
        if form.is_valid():
            new_track = form.save(commit=False)

            # Get file name
            file_name = request.FILES['file'].name

            # If file exists then raise error
            if os.path.isfile('/home/pi/sand-table/media/tracks/' + file_name):
                raise ValidationErr('File already exists')
            
            # If file is not .thr then raise error
            if '.thr' not in file_name:
                raise ValidationErr('Not a thr file')

            name = file_name.split('.')
            new_track.name = name[0]
            new_track.save()
            # Generate image from .thr and return track length
            new_track.track_length = thr2png.drawFile('/home/pi/sand-table/media/tracks/' + file_name)
            new_track.pic = 'tracks/' + name[0] + '.png'
            new_track.save()
            
            return redirect('sandtable:tracks')

    else:
        form = UploadFileForm()

    context = { 'form': form }

    return render(request, 'sandtable/uploadTracks.html', context)


def settings(request):
    settings_data = Settings.objects.get(id=0)
    motor_speed = settings_data.motor_speed

    led_data = RGBW.objects.get(id=1)
    led_speed = led_data.led_speed
    led_intensity = led_data.led_intensity
    led_saturation = led_data.led_saturation

    # Button to home motors
    if(request.GET.get('homeButton')):
        checkProcesses("PlayQueue.py", True)
        writeSerial.writeToSerial(COMMAND_HOME, 0)
        settings_data.r = 0.0
        settings_data.theta = 0.0
        settings_data.save()
        # print(settings_data.calibrate)

        writeSerial.writeToSerial(COMMAND_M2_STEP, settings_data.calibrate)
        
        return redirect('sandtable:settings')

    # Button to calibrate
    if(request.GET.get('calibrateButton')):
        return redirect('sandtable:calibrate')

    # Button to zero
    if(request.GET.get('updateButton')):
        checkProcesses("PlayQueue.py", True)
        r = settings_data.r
        theta = settings_data.theta
        writeSerial.writeToSerial(COMMAND_UPDATE_ANGLE, theta)
        writeSerial.writeToSerial(COMMAND_UPDATE_R, r)

        return redirect('sandtable:settings')

    # Button to stop motors
    elif(request.GET.get('stopButton')):
        checkProcesses("PlayQueue.py", True)
        writeSerial.writeToSerial(COMMAND_STOP, 0)
        return redirect('sandtable:settings')

    # Button to power down the pi
    elif(request.GET.get('powerButton')):
        writeSerial.writeToSerial(COMMAND_GET_ANGLE, 0)
        angle = writeSerial.waitForResponse()
        writeSerial.writeToSerial(COMMAND_GET_R, 0)
        r = writeSerial.waitForResponse()
        settings_data.theta = float(angle)
        settings_data.r = float(r)
        os.system("sudo shutdown -h")
        return redirect('sandtable:settings')
    
    # Button to reset core 1
    elif(request.GET.get('resetButton')):
        writeSerial.writeToSerial(COMMAND_RESET, 0)
        return redirect('sandtable:settings')

    # Button to do led fade
    elif(request.GET.get('fadeButton')):
        led_data.led_track = 5
        led_data.save()
        writeSerial.writeToSerial(COMMAND_LED_SPEED, int(led_data.led_speed))
        writeSerial.writeToSerial(COMMAND_LED_INTENSITY, float(led_data.led_intensity) / 100.0)
        writeSerial.writeToSerial(COMMAND_LED_SATURATION, float(led_data.led_saturation) / 100.0)
        writeSerial.writeToSerial(COMMAND_LED_TRACK, 5)
        return redirect('sandtable:settings')

    # Motor speed slider
    elif(request.GET.get('motorSpeedSlider')):
        value = request.GET.get('motorSpeedSlider')
        settings_data.motor_speed = int(value)
        settings_data.save()
        writeSerial.writeToSerial(COMMAND_MOTOR_SPEED, int(value))

        return redirect('sandtable:settings')

    # Led speed slider
    elif(request.GET.get('ledSpeedSlider')):
        value = request.GET.get('ledSpeedSlider')
        led_data.led_speed = int(value)
        led_data.save()
        writeSerial.writeToSerial(COMMAND_LED_SPEED, int(value))

        return redirect('sandtable:settings')

    # Led intensity slider
    elif(request.GET.get('ledIntensitySlider')):
        value = request.GET.get('ledIntensitySlider')
        led_data.led_intensity = int(value)
        led_data.save()
        writeSerial.writeToSerial(COMMAND_LED_INTENSITY,float(value) / 100.0)

        return redirect('sandtable:settings')

    # Led saturation slider
    elif(request.GET.get('ledSaturationSlider')):
        value = request.GET.get('ledSaturationSlider')
        led_data.led_saturation = int(value)
        led_data.save()
        writeSerial.writeToSerial(COMMAND_LED_SATURATION, float(value) / 100.0)

        return redirect('sandtable:settings')

    context = { 'motorSpeed': motor_speed, 'ledSpeed': led_speed, 'ledIntensity': led_intensity, 'ledSaturation': led_saturation }

    return render(request, 'sandtable/settings.html', context)

def calibrate(request):
    settings_data = Settings.objects.get(id=0)
    
    # Button to calibrate
    if(request.GET.get('+1')):
        writeSerial.writeToSerial(COMMAND_M2_STEP, 1)
        settings_data.calibrate += 1
        settings_data.save()
        return redirect('sandtable:calibrate')

    # Button to calibrate
    if(request.GET.get('-1')):
        writeSerial.writeToSerial(COMMAND_M2_STEP, -1)
        settings_data.calibrate -= 1
        settings_data.save()
        return redirect('sandtable:calibrate')
    
    # Button to calibrate
    if(request.GET.get('+10')):
        writeSerial.writeToSerial(COMMAND_M2_STEP, 10)
        settings_data.calibrate += 10
        settings_data.save()
        return redirect('sandtable:calibrate')

    # Button to calibrate
    if(request.GET.get('-10')):
        writeSerial.writeToSerial(COMMAND_M2_STEP, -10)
        settings_data.calibrate -= 10
        settings_data.save()
        return redirect('sandtable:calibrate')

    return render(request, 'sandtable/calibrate.html')

@csrf_exempt
def color(request):

    # If entry in the table does not exist, then create new with id=1
    if not RGBW.objects.filter(id=1).exists():
        if request.method == 'POST':
            newcolor = RGBW()
            newcolor.id = 1

            # If POST contains RGB or WHITE
            if 'r' in request.POST:
                r = request.POST.get('r')
                newcolor.r = r
                g = request.POST.get('g')
                newcolor.g = g
                b = request.POST.get('b')
                newcolor.b = b
            elif 'w' in request.POST:
                w = request.POST.get('w')
                newcolor.w = w
            
            # Don't know why I thought that I needed this
            newcolor.changed = True
            newcolor.save()

            writeSerial.writeToSerial(COMMAND_LED_R, newcolor.r)
            writeSerial.writeToSerial(COMMAND_LED_G, newcolor.g)
            writeSerial.writeToSerial(COMMAND_LED_B, newcolor.b)
            writeSerial.writeToSerial(COMMAND_LED_W, newcolor.w)

            return HttpResponse('')

        else:
            context = { 'colors': {'r':0, 'g':0, 'b':0, 'w':0} }
            return render(request, 'sandtable/colorPicker.html', context)

    else:
        colors = RGBW.objects.get(id=1)

        if request.method == 'POST':

            # If POST contains RGB or WHITE
            if 'r' in request.POST:
                r = request.POST.get('r')
                colors.r = r
                g = request.POST.get('g')
                colors.g = g
                b = request.POST.get('b')
                colors.b = b
            elif 'w' in request.POST:
                w = request.POST.get('w')
                colors.w = w
            
            # Don't know why I thought that I needed this
            colors.changed = True
            colors.save()

            writeSerial.writeToSerial(COMMAND_LED_R, int(colors.r))
            writeSerial.writeToSerial(COMMAND_LED_G, int(colors.g))
            writeSerial.writeToSerial(COMMAND_LED_B, int(colors.b))
            writeSerial.writeToSerial(COMMAND_LED_W, int(colors.w))

            return HttpResponse('')
        
        else:
            context = { 'colors': colors }
            return render(request, 'sandtable/colorPicker.html', context)


# Find if process is running and killit if needed
def checkProcesses(name, kill=False):
    for proc in psutil.process_iter(attrs=["pid", "name", "cmdline"]):
        try:
            # Check if process name contains the given name string.
            if "python" in proc.name():
                for cmd in proc.cmdline():
                    if name.lower() in cmd.lower():

                        # Kill the process if kill=True
                        if kill:
                            proc.kill()

                        return proc.pid
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
    return False

