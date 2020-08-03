from django.shortcuts import render

# Create your views here.

def tohome(request):
    return render(request,'index.html')

def about(request):
     return render(request,'about.html')

def contact(request):
    return render(request,'contact.html')

def canids(request):
    return render(request,'canids.html')

def index(request):
    return render(request,'index.html')

def gateway(request):
    return render(request,'gateway.html')

def blog(request):
    return render(request,'blog.html')