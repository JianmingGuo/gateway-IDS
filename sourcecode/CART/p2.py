import pandas as pd
import csv

csvFile = open('D:\\jupyter files\\Malfunction_CHEVROLET_Spark_train5.csv', 'r')
reader = csv.reader(csvFile)

ls = []
for item in reader:
    ls.append(item)
csvFile.close()
# ls[1:] 是数据项， ls[0] 是columns
df = pd.DataFrame(data=ls[1:], columns=ls[0])
df=pd.DataFrame(df.values, columns = ["Time","ID","Num","B1","B2","B3","B4","B5","B6","B7","B8","RT"])
df_id_count=df['ID'].value_counts().to_frame()
df_id=df_id_count.index.values.tolist()[0:17]
import numpy as np
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn import metrics
import matplotlib.pyplot as plt
featureNames = list(df.columns.values)[2:11]
for num in range(0, 17):
    df_n = df[df['ID'] == df_id[num]]
    X = df_n.drop(df_n.columns[[0, 1, 11]], axis=1).values
    y = df_n["RT"]

    X_trainset, X_testset, y_trainset, y_testset = train_test_split(X, y, test_size=0.3, random_state=3)

    skullsTree = DecisionTreeClassifier(criterion="gini")
    skullsTree.fit(X_trainset, y_trainset)
    predTree = skullsTree.predict(X_testset)

    print("ID:", df_id[num], "\n", "DecisionTrees's Accuracy: "), print(metrics.accuracy_score(y_testset, predTree))
    print("confusion_matrix:\n ", metrics.confusion_matrix(y_testset, predTree, labels=None, sample_weight=None), "\n")