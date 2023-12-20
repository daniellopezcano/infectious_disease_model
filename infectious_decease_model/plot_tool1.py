# -*- coding: utf-8 -*-
"""
Created on Tue May 26 22:08:23 2020

@author: Daniel
"""

# =========================================================================== #
# ------------------------- IMPORTS AND FUNCTIONS---------------------------- #
# =========================================================================== #

import numpy as np
import pickle
import os
import pandas as pd
import matplotlib.pyplot as plt
import subprocess as sp
import imageio
import glob

plt.style.use('dark_background')
plt.close('all')

root = os.getcwd()

path_to_results = os.path.join(root, 'PLOTS')

output_dirname = os.path.join(root, 'PLOTS1')

list_file_names = ['RESULTS_ALERT_25_65_COLLAPSE_002.pickle',
                   'RESULTS_ALERT_25_65_COLLAPSE_003.pickle',
                   'RESULTS_ALERT_25_65_COLLAPSE_004.pickle',
                   'RESULTS_ALERT_25_65_COLLAPSE_005.pickle',
                   'RESULTS_ALERT_25_65_COLLAPSE_006.pickle',
                   'RESULTS_ALERT_25_65_COLLAPSE_008.pickle',
                   'RESULTS_ALERT_25_65_COLLAPSE_010.pickle']

maxday = 150

days = days=np.linspace(0, maxday-1, maxday)
reduceddays  =np.linspace(0, maxday-2, maxday-1)


MEANs={}
STDs={}
MEDIAN_ALERT={}
MEDIAN_HOSP_COLLAPSE={}

new_deaths={}

for file_name in list_file_names:
    
    with open(os.path.join(path_to_results, file_name), 'rb') as handle:
        [MEANs[file_name], STDs[file_name], MEDIAN_ALERT[file_name], MEDIAN_HOSP_COLLAPSE[file_name]] = pickle.load(handle)
    
    new_deaths[file_name] = np.zeros(maxday-1)
    
    for i in range(maxday-1):
        new_deaths[file_name][i] = MEANs[file_name][4][i + 1] - MEANs[file_name][4][i]

# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #

list_colors = ['dodgerblue','yellow','blueviolet','springgreen','red', 'w', 'blueviolet']

fig = plt.figure(figsize=(20,6))
ax = fig.add_subplot(111)
for i_file_name, file_name in enumerate(list_file_names):
    
    ax.plot(reduceddays, new_deaths[file_name], label='hosp collapse ' + file_name[29:32], c=list_colors[i_file_name])

ax.set_xlim([0, maxday])
# ax.set_ylim([0, max(new_deaths[file_name])+1])
ax.set_xlabel('Days', size=15)
ax.set_ylabel('# daily deads / 100000', size=20)
ax.set_title('# daily deads / 100000 vs days', size=20)
ax.grid(True)
ax.legend(fontsize=20)

# --------------------------------------------------------------------------- #

for file_name in list_file_names:
    
    fig = plt.figure(figsize=(20,6))
    ax = fig.add_subplot(111)
    
    ax.plot(reduceddays, new_deaths[file_name], label='Daily deads')
    ax.plot(days, max(new_deaths[file_name])*MEDIAN_HOSP_COLLAPSE[file_name], 'x', c ='r',  label='hospital collapse')

    ax.set_xlim([0, maxday])
    ax.set_ylim([0, 210])
    ax.set_xlabel('Days', size=20)
    ax.set_ylabel('# Daily deads / 100000', size=20)
    ax.set_title(file_name)
    ax.grid(True)
    ax.legend(fontsize=20, loc='upper left')

    fig.savefig( os.path.join(output_dirname, 'daily_deaths_' + file_name + '.png') )

# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #
'''
import matplotlib.pylab as pl
colors = pl.cm.autumn(np.linspace(0,1,len(list_file_names)))

state = 4

fig = plt.figure(figsize=(18,6))
ax = fig.add_subplot(111)
for i_file_name, file_name in enumerate(list_file_names):
    
    ax.plot(days, MEANs[file_name][state], label=file_name[29:32], c=colors[i_file_name], lw=2)
    ax.fill_between(days, MEANs[file_name][state]-STDs[file_name][state]/2, MEANs[file_name][state]+STDs[file_name][state]/2, alpha=0.4, color = colors[i_file_name], linewidth=2)
    
ax.set_xlim([0, maxday])
ax.set_ylim([0, 21100])
ax.set_xlabel('Days')
ax.set_ylabel('# deads / 100000')
ax.set_title(r'Alert $\in [25, 65]$')
ax.grid(True)
ax.legend()

'''













plt.show()
