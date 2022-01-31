from django.db import models


# Creating track entries
class Tracks(models.Model):
    file = models.FileField(upload_to='tracks/')
    name = models.CharField(max_length=200)
    pic = models.ImageField()
    track_length = models.IntegerField(default=0)
    uploaded_at = models.DateTimeField(auto_now_add=True)


# Creating entries for the queue
class Queue(models.Model):
    file = models.CharField(max_length=200)
    track_length = models.IntegerField()
    pic = models.CharField(max_length=200)


# Putting LED strip values to database
class RGBW(models.Model):
    r = models.IntegerField(default=0)
    g = models.IntegerField(default=0)
    b = models.IntegerField(default=0)
    w = models.IntegerField(default=0)
    led_track = models.CharField(max_length=100, default='None')
    changed = models.BooleanField(default=False)