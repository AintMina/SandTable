# Generated by Django 4.0 on 2022-10-16 11:57

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('sandtable', '0004_rgbw_led_intensity_rgbw_led_saturation_and_more'),
    ]

    operations = [
        migrations.AddField(
            model_name='settings',
            name='r',
            field=models.FloatField(default=0.0),
        ),
        migrations.AddField(
            model_name='settings',
            name='theta',
            field=models.FloatField(default=0.0),
        ),
    ]
