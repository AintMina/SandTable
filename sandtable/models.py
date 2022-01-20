from django.db import models

# Create your models here.
class Tracks(models.Model):
    file = models.FileField(upload_to='tracks/')
    name = models.CharField(max_length=200)
    pic = models.ImageField()
    track_length = models.IntegerField(default=0)
    uploaded_at = models.DateTimeField(auto_now_add=True)