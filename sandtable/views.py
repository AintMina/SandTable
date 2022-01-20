from xml.dom import ValidationErr
from django.shortcuts import render, redirect
from .models import *
from .forms import *
import thr2png, os


# Create your views here.
def index(request):
    return render(request, 'sandtable/index.html')


def tracks(request):
    tracks = Tracks.objects.order_by('uploaded_at')
    context = { 'tracks': tracks }

    return render(request, 'sandtable/tracks.html', context)


def track(request, track_id):
    track = Tracks.objects.get(id=track_id)
    context = { 'track': track }

    return render(request, 'sandtable/track.html', context)


def uploadTracks(request):
    if request.method == 'POST':
        form = UploadFileForm(request.POST, request.FILES)
        if form.is_valid():
            new_track = form.save(commit=False)
            # Get file name
            file_name = request.FILES['file'].name

            if os.path.isfile('/home/pi/prototype/media/tracks/' + file_name):
                raise ValidationErr('File already exists')
            if '.thr' not in file_name:
                raise ValidationErr('Not a thr file')

            name = file_name.split('.')
            new_track.name = name[0]
            new_track.save()
            # Generate image from .thr and return track length
            new_track.track_length = thr2png.drawFile('/home/pi/prototype/media/tracks/' + file_name)
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
    return render(request, 'sandtable/settings.html')


def color(request):
    return render(request, 'sandtable/colorPicker.html')