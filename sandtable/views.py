from xml.dom import ValidationErr
from django.shortcuts import render, redirect
from .models import *
from .forms import *
import thr2png2, os, psutil, writeSerial


# Create your views here.
def index(request):
    return render(request, 'sandtable/index.html')


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


def color(request):
    return render(request, 'sandtable/colorPicker.html')


def checkProcesses(name):
    for proc in psutil.process_iter(attrs=["pid", "name", "cmdline"]):
        try:
            # Check if process name contains the given name string.
            if "python" in proc.name():
                for cmd in proc.cmdline():
                    if name.lower() in cmd.lower():
                        return True
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
    return False

