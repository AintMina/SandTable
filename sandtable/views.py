from ctypes.wintypes import RGB
from xml.dom import ValidationErr
from django.shortcuts import render, redirect
from .models import *
from .forms import *
import thr2png2, os, psutil, writeSerial

from django.http import HttpResponse
import json
from django.views.decorators.csrf import csrf_exempt


# Create your views here.
def index(request):
    playing = checkProcesses("PlayQueue.py")

    context = { 'playing': playing}
    return render(request, 'sandtable/index.html', context)


def tracks(request):
    tracks = Tracks.objects.order_by('uploaded_at')
    context = { 'tracks': tracks }

    return render(request, 'sandtable/tracks.html', context)


def track(request, track_id):
    track = Tracks.objects.get(id=track_id)

    # Add track to queue
    if request.method == 'POST':
        form = AddToQueueForm(request.POST)
        if form.is_valid():
            new_queue_track = form.save(commit=False)
            new_queue_track.file = track.file
            new_queue_track.track_length = track.track_length
            new_queue_track.pic = track.pic
            new_queue_track.save()

            # Start playing
            # if not checkProcesses("PlayQueue.py"):
            #     os.system("/bin/python /home/pi/sand-table/PlayQueue.py &")
            
            return redirect('sandtable:tracks')
    
    else:
        form = UploadFileForm()

    context = { 'track': track, 'form': form }

    return render(request, 'sandtable/track.html', context)


def queue(request):
    queue = Queue.objects.all()
    context = { 'queue': queue }

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

            if os.path.isfile('/home/pi/sand-table/media/tracks/' + file_name):
                raise ValidationErr('File already exists')
            if '.thr' not in file_name:
                raise ValidationErr('Not a thr file')

            name = file_name.split('.')
            new_track.name = name[0]
            new_track.save()
            # Generate image from .thr and return track length
            new_track.track_length = thr2png2.drawFile('/home/pi/sand-table/media/tracks/' + file_name)
            new_track.pic = 'tracks/' + name[0] + '.png'
            new_track.save()
            
            return redirect('sandtable:tracks')

    else:
        form = UploadFileForm()

    context = { 'form': form }

    return render(request, 'sandtable/uploadTracks.html', context)


def editTrack(request, track_id):
    track = Tracks.objects.get(id=track_id)

    return render(request, 'sandtable/editTrack.html')


def settings(request):

    # Button to home motors
    if(request.GET.get('homeButton')):
        writeSerial.writeToSerial("c home")
        return redirect('sandtable:settings')

    return render(request, 'sandtable/settings.html')


@csrf_exempt
def color(request):
    if not RGBW.objects.filter(id=1).exists():
        if request.method == 'POST':
            newcolor = RGBW()
            newcolor.id = 1
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
            newcolor.changed = True
            newcolor.save()

            values = "{} {} {} {}".format(newcolor.r, newcolor.g, newcolor.b, newcolor.w)
            writeSerial.writeToSerial("lc " + values)

            return HttpResponse('')

        else:
            context = { 'colors': {'r':0, 'g':0, 'b':0, 'w':0} }
            return render(request, 'sandtable/colorPicker.html', context)

    else:
        colors = RGBW.objects.get(id=1)

        if request.method == 'POST':
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
            colors.changed = True
            colors.save()

            values = "{} {} {} {}".format(colors.r, colors.g, colors.b, colors.w)
            print(values)
            writeSerial.writeToSerial("lc " + values)

            return HttpResponse('')
        
        else:
            context = { 'colors': colors }
            return render(request, 'sandtable/colorPicker.html', context)


def checkProcesses(name, kill=False):
    for proc in psutil.process_iter(attrs=["pid", "name", "cmdline"]):
        try:
            # Check if process name contains the given name string.
            if "python" in proc.name():
                for cmd in proc.cmdline():
                    if name.lower() in cmd.lower():
                        if kill:
                            proc.kill()

                        return True
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
    return False

