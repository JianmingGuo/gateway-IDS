# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Numenta Platform for Intelligent Computing (NuPIC)
# Copyright (C) 2013-2018, Numenta, Inc.  Unless you have an agreement
# with Numenta, Inc., for a separate license for this software code, the
# following terms and conditions apply:
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU Affero Public License for more details.
#
# You should have received a copy of the GNU Affero Public License
# along with this program.  If not, see http://www.gnu.org/licenses.
#
# http://numenta.org/licenses/
# ----------------------------------------------------------------------
"""
This example shows how to use the `SDRCategoryEncoder` with `HTMPredictionModel`
to analyze web site traffic data by extracting temporal patterns from user
sessions described as a sequences of web page categories.

We will use the [MSNBC.com Anonymous Web Data][1] data set provided by
[UCI Machine Learning Repository][2] to predict the next page the user is more
likely to click. In this data set each page is assigned a category and the user
behavior is recorded as navigating from one page to another.

Dataset characteristics:

  - Number of users: 989,818
  - Average number of visits per user: 5.7
  - Number of categories: 17
  - Number of URLs per category: 10 to 5,000

See [dataset][1] description for more information.

References:

  1. https://archive.ics.uci.edu/ml/datasets/MSNBC.com+Anonymous+Web+Data
  2. Lichman, M. (2013). UCI Machine Learning Repository [http://archive.ics.uci.edu/ml].
     Irvine, CA: University of California, School of Information and Computer Science
"""
import os
import random
import sys
import zipfile
from operator import itemgetter

import numpy as np
import prettytable
from prettytable import PrettyTable

from nupic.frameworks.opf.model_factory import ModelFactory
import csv
import matplotlib.pyplot as plt

# List of page categories used in the dataset
# PAGE_CATEGORIES = [
#   "frontpage", "news", "tech", "local", "opinion", "on-air", "misc", "weather",
#   "msn-news", "health", "living", "business", "msn-sports", "sports", "summary",
#   "bbs", "travel"
# ]
PAGE_CATEGORIES_1 = [
  '04f2', '00a0', '0370', '05a2', '0690', '0110', '04b0', '02b0', '05a0', '0165', '0081', '018f', '02a0', '0220', '0080', '01f1', '0120', '0153', '0260', '0545', '04f1', '0043', '059b', '0587', '05e4', '05f0', '04b1', '0329', '0382', '043f', '0510', '02c0', '051a', '0050', '00a1', '0018', '0034', '0044', '0440', '0042', '04f0', '0517', '0164', '0350', '0316'
]
PAGE_CATEGORIES = [
'02A0', '0130', '0131', '0690', '00A1', '00A0', '02C0', '043F', '0545', '0329', '0140', '0440', '04B1', '0260', '0002', '04F0', '05F0', '018F', '02B0', '01F1', '0350', '0430', '0370', '0153', '0316', '05A2', '05A0'
]
# PAGE_CATEGORIES_PRE = [
#   '04f2', '00a0', '0370', '05a2', '0690', '0110', '04b0', '02b0', '05a0', '0165', '0081', '018f', '02a0', '0220', '0080', '01f1', '0120', '0153', '0260', '0545', '04f1', '0043', '059b', '0587', '05e4', '05f0', '04b1', '0329', '0382', '043f', '0510', '02c0', '051a', '0050', '00a1', '0018', '0034', '0044', '0440', '0042', '04f0', '0517', '0164', '0350', '0316'
# ]

# PAGE_CATEGORIES = []
# for i1 in PAGE_CATEGORIES_PRE:
#   for i2 in range(3):
#     i3 = i1+'_'+str(i2)
#     PAGE_CATEGORIES.append(i3)

# print(PAGE_CATEGORIES)
# Configure the sensor/input region using the "SDRCategoryEncoder" to encode
# the page category into SDRs suitable for processing directly by the TM
SENSOR_PARAMS = {
  "verbosity": 0,
  "encoders": {
    "page": {
      "fieldname": "page",
      "name": "page",
      "type": "SDRCategoryEncoder",
      # The output of this encoder will be passed directly to the TM region,
      # therefore the number of bits should match TM's "inputWidth" parameter
      "n": 1024,
      # Use ~2% sparsity
      "w": 21

    },
  },
}

# Configure the temporal memory to learn a sequence of page SDRs and make
# predictions on the next page of the sequence.
TM_PARAMS = {
  "seed": 1960,
  # Use "nupic.bindings.algorithms.TemporalMemoryCPP" algorithm
  "temporalImp": "tm_cpp",
  # Should match the encoder output
  "inputWidth": 1024,
  "columnCount": 2048,
  # Use 1 cell per column for first order prediction.
  # Use more cells per column for variable order predictions.
  "cellsPerColumn": 8,
}

# Configure the output region with a classifier used to decode TM SDRs back
# into pages
CL_PARAMS = {
  "implementation": "cpp",
  "regionName": "SDRClassifierRegion",
  # alpha parameter controls how fast the classifier learns/forgets. Higher
  # values make it adapt faster and forget older patterns faster.
  "alpha": 0.1,
  "steps": 1,
}

# Create a simple HTM network that will receive the current page as input, pass
# the encoded page SDR to the temporal memory to learn the sequences and
# interpret the output SDRs from the temporary memory using the SDRClassifier
# whose output will be a list of predicted next pages and their probabilities.
#
#   page => [encoder] => [TM] => [classifier] => prediction
#
MODEL_PARAMS = {
  "version": 1,
  "model": "HTMPrediction",
  "modelParams": {
    # 'anomalyParams': { u'anomalyCacheRecords': None,
    #                    u'autoDetectThreshold': None,
    #                    u'autoDetectWaitRecords': None},

    "inferenceType": "TemporalAnomaly",

    "sensorParams": SENSOR_PARAMS,

    # The purpose of the spatial pooler is to create a stable representation of
    # the input SDRs. In our case the category encoder output is already a
    # stable representation of the category therefore adding the spatial pooler
    # to this network would not help and could potentially slow down the
    # learning process
    "spEnable": False,
    "spParams": {},

    "tmEnable": True,
    "tmParams": TM_PARAMS,

    "clParams": CL_PARAMS,
  },
}

# Learn page sequences from the first 10,000 user sessions.
# We chose 10,000 because it gives results that are good enough for this example
# Use more records for learning to improve the prediction accuracy
LEARNING_RECORDS = 11000

def calc(i,win,copy):
  p = win - 1
  re = 0
  while (p >= 0):
    re += copy[i - p]
    p -= 1
  return re

def computeAccuracy(model, size, top):
  """
  Compute prediction accuracy by checking if the next page in the sequence is
  within the top N predictions calculated by the model
  Args:
    model: HTM model
    size: Sample size
    top: top N predictions to use

  Returns: Probability the next page in the sequence is within the top N
           predicted pages
  """
  accuracy = []

  # Load MSNBC web data file

  with open("raw.csv") as datafile:
    # Skip header lines (first 7 lines)
    next(datafile)
    reader = csv.reader(datafile)
    pages = [(row[1:2][0]) for row in reader][50000:60000]
    for i in range(len(pages)-1):
        result = model.run({"page": pages[i]})
        inferences = result.inferences["multiStepPredictions"][1]

        # Get top N predictions for the next page
        predicted = sorted(inferences.items(), key=itemgetter(1), reverse=True)[:top]
        real = pages[i + 1]
        real_part = real[:4]
        cp_predicted = []
        for i1 in predicted:
          cp_predicted.append(i1[0][:4])
        # Check if the next page is within the predicted pages
        if real in zip(*predicted)[0]:
          accuracy.append(1)
        elif real_part in cp_predicted:
          accuracy.append(0.5)
        else:
          accuracy.append(0)

    copy = []
    win = 100
    for i1 in accuracy:
      copy.append(i1)  # copy是anomaly_list的复制版
    if win:
      top = len(copy)
      i = win
      while (i < top):
        accuracy[i] = calc(i, win, copy)
        i = i + 1

        # 下面是画图模块
    X1 = [i for i in range(len(accuracy))]
    plt.figure(figsize=(16, 6), dpi=80)
    plt.figure(1)
    ax1 = plt.subplot(211)
    plt.plot(X1, accuracy)
    plt.show()

  return np.mean(accuracy)

def anomaly_output(model):

  anomaly_list = []
  win = 100
  attack_point = 0
  attack_end = 0
  attack_dic = {}
  switch = 0
  attack_prob = 0
  # Load MSNBC web data file
  with open("raw.csv") as datafile:
    # csvfile = file('anomaly.csv', 'w')
    # writer = csv.writer(csvfile)
    next(datafile)
    reader = csv.reader(datafile)
    pages = [(row[1]) for row in reader][50000:170000]
    scores_list = []
    for i in range(len(pages)-1):
        result = model.run({"page": pages[i]})
        inferences = result.inferences["multiStepPredictions"][1]
        # predicted = sorted(inferences.items(), key=itemgetter(1), reverse=True)[:3]
        # real = pages[i+1]
        anomalyScore = result.inferences["anomalyScore"]
        anomaly_list.append(anomalyScore)
        # writer.writerow([pages[i], anomalyScore])
        l1 = len(anomaly_list)
        scores = 0
        if l1 > win:
          for j in range(win):
            scores += anomaly_list[l1-j-1]
          scores_list.append(scores)
          if len(scores_list) > 1500:
            if not switch:
              history = np.mean(scores_list[-1500:-500])
            if scores_list[-1] > 1.5 * history:
              if attack_point == 0:
                attack_point = len(scores_list)
                attack_prob = 0
                switch = 1
              else:
                attack_prob += 1
            else:
              attack_point = 0
              switch = 1
              attack_end = 0
              attack_prob = 0
          if attack_prob >= 1000:
            attack_end = len(scores_list)
          if attack_end:
            attack_dic[attack_point] = attack_end


        sys.stdout.write("\rInferenced {} Sessions".format(i + 1))
        sys.stdout.flush()

  # 下面是画图模块
  X1 = [i for i in range(len(scores_list))]
  plt.figure(figsize=(16, 6), dpi=80)
  plt.figure(1)
  ax1 = plt.subplot(211)
  plt.plot(X1, scores_list)
  plt.show()

  print
  print(np.mean(scores_list))
  print(np.var(scores_list))
  print(np.std(scores_list))
  maxindex = scores_list.index(max(scores_list))
  print ("Intrude point: ",maxindex)
  print ("Anomaly scores: ",max(scores_list))
  print(attack_dic)

def readUserSession(datafile):
  """
  Reads the user session record from the file's cursor position
  Args:
    datafile: Data file whose cursor points at the beginning of the record

  Returns:
    list of pages in the order clicked by the user
  """
  for line in datafile:
    pages = line.split()
    total = len(pages)
    # Select user sessions with 2 or more pages
    if total < 2:
      continue

    # Exclude outliers by removing extreme long sessions
    if total > 500:
      continue

    return [PAGE_CATEGORIES[int(i) - 1] for i in pages]
  return []

def main():
  # Create HTM prediction model and enable inference on the page field
  model = ModelFactory.create(MODEL_PARAMS)
  model.enableInference({"predictedField": "page"})

  # Use the model encoder to display the encoded SDRs the model will learn
  sdr_table = PrettyTable(field_names=["Page Category",
                                       "Encoded SDR (on bit indices)"],
                          sortby="Page Category")
  sdr_table.align = "l"

  with open("raw.csv") as datafile:
    # Skip header lines (first 7 lines)

    next(datafile)
    reader = csv.reader(datafile)
    pages = []
    for row in reader:
      try:
        pages.append(row[1])
      except:
        pass
    pages_set = set(pages)
    print pages_set
    print
    print "Start learning page sequences using the first {} user " \
          "sessions".format(LEARNING_RECORDS)
    model.enableLearning()
    for count in range(LEARNING_RECORDS):
      model.run({"page": pages[count]})

      # Simple progress status
      sys.stdout.write("\rLearned {} Sessions".format(count + 1))
      sys.stdout.flush()

    print "\nFinished learning"
    model.disableLearning()
  # accuracy = computeAccuracy(model, 100, 3)
  anomaly_output(model)

    # Use the new HTM model to predict next user session
    # The test data starts right after the learning data
    # print
    # print "Start Inference using a new user session from the dataset"
    # prediction_table = PrettyTable(field_names=["Page", "Prediction"],
    #                                hrules=prettytable.ALL)
    # prediction_table.align["Prediction"] = "l"
    #
    # # Infer one page of the sequence at the time
    # model.resetSequenceStates()
    # # session = readUserSession(datafile)
    # for page in pages[LEARNING_RECORDS+1:]:
    #   result = model.run({"page": page})
    #   inferences = result.inferences["multiStepPredictions"][1]
    #
    #   # Print predictions ordered by probabilities
    #   predicted = sorted(inferences.items(),
    #                      key=itemgetter(1),
    #                      reverse=True)
    #   prediction_table.add_row([page, zip(*predicted)[0]])
    # predicted = sorted(inferences.items(),
    #                    key=itemgetter(1),
    #                    reverse=True)
    #   prediction_table.add_row([page, zip(*predicted)[0]])
    # print "User Session to Predict: ", session
    # print prediction_table

  # print
  # print "Compute prediction accuracy by checking if the next page in the " \
  #       "sequence is within the predicted pages calculated by the model:"
  # accuracy = computeAccuracy(model, 100, 1)
  # print " - Prediction Accuracy:", accuracy

  # print accuracy
  # print " - Accuracy Predicting Top 3 Pages:", accuracy

if __name__ == "__main__":
  random.seed(1)
  np.random.seed(1)
  main()
