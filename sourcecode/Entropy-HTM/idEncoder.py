import os
import random
import sys
import zipfile
from operator import itemgetter

import numpy
import numpy as np
import prettytable
from prettytable import PrettyTable

from nupic.frameworks.opf.model_factory import ModelFactory
import csv
import matplotlib.pyplot as plt

PAGE_CATEGORIES = [
  '04f2', '00a0', '0370', '05a2', '0690', '0110', '04b0', '02b0', '05a0', '0165', '0081', '018f', '02a0', '0220', '0080', '01f1', '0120', '0153', '0260', '0545', '04f1', '0043', '059b', '0587', '05e4', '05f0', '04b1', '0329', '0382', '043f', '0510', '02c0', '051a', '0050', '00a1', '0018', '0034', '0044', '0440', '0042', '04f0', '0517', '0164', '0350', '0316'
]

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
            "n": 512,
            # Use ~2% sparsity
            "w": 12
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
    "inputWidth": 512,
    "columnCount": 2048,
    # Use 1 cell per column for first order prediction.
    # Use more cells per column for variable order predictions.
    "cellsPerColumn": 6,
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


def main():
    # Create HTM prediction model and enable inference on the page field
    model = ModelFactory.create(MODEL_PARAMS)
    model.enableInference({"predictedField": "page"})

    # Use the model encoder to display the encoded SDRs the model will learn
    sdr_table = PrettyTable(field_names=["Page Category",
                                         "Encoded SDR (on bit indices)"],
                            sortby="Page Category")
    sdr_table.align = "l"

    encoder = model._getEncoder()


    sdrout = np.zeros(encoder.getWidth(), dtype=np.bool)

    for page in PAGE_CATEGORIES:
        encoder.encodeIntoArray({"page": page}, sdrout)
        sdr_table.add_row([page, sdrout.nonzero()[0]])

    sdrlist = encoder.getsdrs()
    numpy.save("idEn.npy", sdrlist)

if __name__ == "__main__":
    random.seed(1)
    np.random.seed(1)
    main()
