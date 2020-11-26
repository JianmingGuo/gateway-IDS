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
from openpyxl import load_workbook

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

        f = open('./canids/upload/testfile.xlsx','wb+')
        for line in file.chunks():
            f.write(line)
        f.close()

    return redirect('/canids/uploadsuccess2/')



def canids_uplaodsuccess1(req):
    return render(req,'canids.html',{"errinf":"successful file!"})


def canids_uplaodsuccess2(req):
    return render(req,'canids.html',{"errinf1":"successful file!"})

def get_xlsx(filename):
    workbook = load_workbook(filename)
    booksheet = workbook.active
    rows = booksheet.rows

    data = []

    for row in rows:
        line = [col.value for col in row]
        data.append(line)
    return data


def GetList(request):

    filename = './canids/upload/testfile.xlsx'
    data = get_xlsx(filename)


    list = []

    if request.is_ajax():
        list.append({'id':str('1'),'model':'basic classifier' })
        list.append({"pid":str('1'),'id':'1_1','model':'LSTM','possibility':str(data[0][1])  })
        list.append({"pid":str('1'),'id':'1_2','model':'ANN','possibility':str(data[1][1])  })
        list.append({"pid":str('1'),'id':'1_3','model':'HTM','possibility':str(data[2][1])  })
        list.append({"pid":str('1'),'id':'1_2','model':'CART','possibility':str(data[3][1])  })
        list.append({"pid":str('1'),'id':'1_2','model':'转移矩阵','possibility':str(data[4][1])  })

        list.append({'id':str('2'),'model':'emseble classifier(possibility:'+data[5][0]+','+str(data[5][1])+')' })
        list.append({"pid":str('2'),'id':'2_1','model':'Attack Type','possibility':'Emsemble results'  })
        list.append({"pid":str('2'),'id':'2_2','model':'DoS','possibility':str(data[6][1])  })
        list.append({"pid":str('2'),'id':'2_3','model':'Fuzzy','possibility':str(data[7][1])  })
        list.append({"pid":str('2'),'id':'2_4','model':'Spoofing','possibility':str(data[8][1])  })
        list.append({"pid":str('2'),'id':'2_5','model':'Replay','possibility':str(data[9][1])  })
            

    re = {'list':json.dumps(list)}
    response = JsonResponse(re)
    return response
