from django.apps import AppConfig
from .models import *


class SandtableConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'sandtable'

    def ready(self):
        # Startup code here
        pass