from unicodedata import name
from django.urls import path, include
from .views import *
from .forms import *

app_name = 'sandtable'
urlpatterns = [
    path('', index, name='index'),
    path('tracks/', tracks, name='tracks'),
    path('settings/', settings, name='settings'),
    path('color/', color, name='color'),
    path('upload/', uploadTracks, name='upload'),
    path('tracks/<int:track_id>', track, name='track')
]