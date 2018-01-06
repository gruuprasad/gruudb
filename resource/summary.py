import pandas as pd
from pandas import DataFrame
import matplotlib.pyplot as plot


df = pd.read_csv('lineitem.tbl', sep='|')
summary = df.describe()
print(summary)

