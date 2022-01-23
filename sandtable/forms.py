from django import forms
from .models import *


class UploadFileForm(forms.ModelForm):
    class Meta:
        model = Tracks
        fields = ('file', )


class AddToQueueForm(forms.ModelForm):
    class Meta:
        model = Queue
        fields = ()