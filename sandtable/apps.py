from django.apps import AppConfig


class SandtableConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'sandtable'

    def ready(self):
        # Startup code here
        print("Starting up...")