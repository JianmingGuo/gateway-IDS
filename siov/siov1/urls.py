"""siov1 URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/3.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from . import view
from canids.view import canids_uplaod_normal,canids_uplaod_test,canids_uplaodsuccess1,canids_uplaodsuccess2,ann

urlpatterns = [
    path('admin/', admin.site.urls),
    path('',view.tohome,name='home'),
    path('index.html/',view.index,name='index'),
    path('gateway.html/',view.gateway,name='gateway'),
    path('canids.html/',view.canids,name='canids'),
    path('blog.html/',view.blog,name='blog'),
    path('contact.html/',view.contact,name='contact'),

    path('canids/upload_normal/',canids_uplaod_normal),
    path('canids/upload_test/',canids_uplaod_test),
    path('canids/uploadsuccess1/',canids_uplaodsuccess1),
    path('canids/uploadsuccess2/',canids_uplaodsuccess2),

    path('canids/ann/',ann),
    # path('about.html/',view.about),
    # path('contact.html/',view.contact),
    # path('gallery.html/',view.gallery),
    # path('index.html/',view.index),
    # path('products.html',view.products),
    # path('services.html/',view.services),
]
