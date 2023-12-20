# -*- coding: utf-8 -*-
"""
Created on Tue Apr  7 18:48:26 2020

@author: Daniel
"""

# =========================================================================== #
# ------------------------- IMPORTS AND FUNCTIONS---------------------------- #
# =========================================================================== #

import matplotlib
matplotlib.use('Agg')
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

output_dirname = os.path.join(root, 'PLOTS')
if not os.path.exists(output_dirname):
    os.makedirs(output_dirname)

Nrealizations = 10


list_name_results = ["RESULTS_NOALERT_COLLAPSE_003",
"RESULTS_NOALERT_COLLAPSE_005",
"RESULTS_ALERT_25_65_COLLAPSE_003",
"RESULTS_ALERT_25_65_COLLAPSE_005",
"RESULTS_ALERT_15_80_COLLAPSE_003",
"RESULTS_ALERT_15_80_COLLAPSE_005"]

list_collapse = [0.03,0.05,0.03,0.05,0.03,0.05]

list_alert_start = [-1,-1,25,25,15,15]

list_alert_end = [-1,-1,65,65,80,80]

for i_name_results, name_results in enumerate(list_name_results):
    
    collapse = list_collapse[i_name_results]
    alert_start = list_alert_start[i_name_results]
    alert_end =list_alert_end[i_name_results]
    
    print('Working with : ', name_results)
    
    path_to_results = os.path.join(root, name_results)

    # =========================================================================== #
    # -------------------------------- LOAD DATA -------------------------------- #
    # =========================================================================== #

    DATA = {}
    for i in range(1, Nrealizations + 1):
        filename = str(i) + '.dat'
        path_to_file = os.path.join(path_to_results, filename)
        print('Loading ', path_to_file)
        DATA[i] = pd.read_csv(path_to_file, sep=" ", header=None)
        [Ndays, N] = DATA[i].shape
        N = N - 3

    days=np.linspace(0, Ndays-1, Ndays).astype(int)

    print('Processing...\n')

    Nstates = 5

    normalized_COUNTS = [np.zeros((Ndays, Nrealizations)), np.zeros((Ndays, Nrealizations)), np.zeros((Ndays, Nrealizations)), np.zeros((Ndays, Nrealizations)), np.zeros((Ndays, Nrealizations))]
    ALERT = np.zeros((Ndays, Nrealizations))
    HOSP_COLLAPSE = np.zeros((Ndays, Nrealizations))
    for ikey, key in enumerate(DATA.keys()):
        for day in days:
            for i in range(Nstates):
                normalized_COUNTS[i][day, ikey] = ((DATA[key].loc[day, 0:N] == i).sum())*100000 / N
            ALERT[day, ikey] = DATA[key].loc[day, N+1]
            HOSP_COLLAPSE[day, ikey] = DATA[key].loc[day, N+2]
            

    MEANs = [np.zeros(Ndays), np.zeros(Ndays), np.zeros(Ndays), np.zeros(Ndays), np.zeros(Ndays)]
    STDs = [np.zeros(Ndays), np.zeros(Ndays), np.zeros(Ndays), np.zeros(Ndays), np.zeros(Ndays)]
    MEDIAN_ALERT = np.zeros(Ndays)
    MEDIAN_HOSP_COLLAPSE = np.zeros(Ndays)
    for day in days:
        for i in range(Nstates):
            MEANs[i][day] = np.mean(normalized_COUNTS[i][day, :])
            STDs[i][day] = np.std(normalized_COUNTS[i][day, :])
        MEDIAN_ALERT[day]  = np.median(ALERT[day, :])
        MEDIAN_HOSP_COLLAPSE[day]  = np.median(HOSP_COLLAPSE[day, :])
    #print(MEDIAN_ALERT)
      
    # =========================================================================== #
    # ----------------------------------- save ---------------------------------- #
    # =========================================================================== #
    
    if not os.path.exists(output_dirname):
        os.makedirs(output_dirname)
    
    pickle_dict = [MEANs, STDs, MEDIAN_ALERT, MEDIAN_HOSP_COLLAPSE]
    with open(os.path.join(output_dirname, name_results + '.pickle'), 'wb') as fp:
        pickle.dump(pickle_dict, fp)
    
    print('data saved\n\n\n\n\n')
    
    # =========================================================================== #
    # -------------------------------- Plotting --------------------------------- #
    # =========================================================================== #

    print('Plotting...\n')

    list_labels = ['S','E','I','R','D']
    list_colors = ['dodgerblue','yellow','blueviolet','springgreen','red']

    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(111)

    for i in range(Nstates):
        ax.plot(days, MEANs[i], c=list_colors[i], label=list_labels[i])
        ax.fill_between(days, MEANs[i]-3*STDs[i]/2, MEANs[i]+3*STDs[i]/2, alpha=0.4, color = list_colors[i], linewidth=2)

    ax.plot(days, MEANs[1] + MEANs[2], c='orangered', label='E + I')
    ax.fill_between(days, MEANs[1] + MEANs[2] - (STDs[1] + STDs[2])/2, MEANs[1] + MEANs[2] + (STDs[1] + STDs[2])/2, alpha=0.4, color = 'orangered', linewidth=2)

    ax.axhline(collapse * 10**5, color='w', lw=2, ls = '--')
    ax.axvline(alert_start, color='r', lw=2, ls = '--')
    ax.axvline(alert_end, color='r', lw=2, ls = '--')

    ax.set_xlim([0, Ndays - 1])
    ax.set_ylim([0, 100000])
    ax.set_xlabel('Days')
    ax.set_ylabel('# of people / 100000')
    ax.grid(True)
    ax.set_title(name_results)
    ax.legend(loc='upper center', bbox_to_anchor=(0.75, 0.95),
              ncol=Nstates, fancybox=True, shadow=True)

    # ----------------------------------- saving ------------------------------- #

    fig.savefig( os.path.join(output_dirname, name_results + '.png') )

    plt.close(fig) 
