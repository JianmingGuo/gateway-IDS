from django.shortcuts import render,redirect
import os
import shutil
import json
from django.shortcuts import render,redirect
import shutil
from django.shortcuts import HttpResponse
import os
from django.http import JsonResponse
import json

from django.views.decorators.csrf import csrf_exempt

def canids_uplaod_normal(req):
    shutil.rmtree('./canids/train')
    os.mkdir('./canids/train')

    print('uploading normal data...')

    if req.method == "POST":
        file = req.FILES.get("upload",None)
        if not file:
            return render(req,"canids.html",{"errinf":"No files for upload!"})

        f = open('./canids/train/train.csv','wb+')
        for line in file.chunks():
            f.write(line)
        f.close()

    return redirect('/canids/uploadsuccess1/')

def canids_uplaod_test(req):
    shutil.rmtree('./canids/upload')
    os.mkdir('./canids/upload')

    print('upload test data...')

    if req.method == "POST":
        file = req.FILES.get("upload",None)
        if not file:
            return render(req,"canids.html",{"errinf1":"No files for upload!"})

        f = open('./canids/upload/testfile.csv','wb+')
        for line in file.chunks():
            f.write(line)
        f.close()

    return redirect('/canids/uploadsuccess2/')



def canids_uplaodsuccess1(req):
    return render(req,'canids.html',{"errinf":"successful file!"})


def canids_uplaodsuccess2(req):
    return render(req,'canids.html',{"errinf1":"successful file!"})


def GetList(request):

    list = []

    if request.is_ajax():
        list.append({'id':str('1'),'model':'basic classifier' })
        list.append({"pid":str('1'),'id':'1_1','model':'LSTM','possibility':'99.15%'  })
        list.append({"pid":str('1'),'id':'1_2','model':'ANN','possibility':'98.32%'  })
        list.append({"pid":str('1'),'id':'1_3','model':'HTM','possibility':'98.14%'  })
        list.append({"pid":str('1'),'id':'1_2','model':'CART','possibility':'98.23%'  })
        list.append({"pid":str('1'),'id':'1_2','model':'转移矩阵','possibility':'98.07%'  })

        list.append({'id':str('2'),'model':'emseble classifier(possibility:98.39%,DOS)' })
        list.append({"pid":str('2'),'id':'2_1','model':'Attack Type','possibility':'Emsemble results'  })
        list.append({"pid":str('2'),'id':'2_2','model':'DoS','possibility':'0.984224'  })
        list.append({"pid":str('2'),'id':'2_3','model':'Fuzzy','possibility':'0.984103'  })
        list.append({"pid":str('2'),'id':'2_4','model':'Spoofing','possibility':'0.984200'  })
        list.append({"pid":str('2'),'id':'2_5','model':'Replay','possibility':'0.983979'  })
            

    re = {'list':json.dumps(list)}
    response = JsonResponse(re)
    return response
