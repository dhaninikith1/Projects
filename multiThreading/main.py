import subprocess
import os
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import seaborn as sns


def main():
    file_name = "sim.c"
    executable_name = "run_sim"
    full_executable_path = os.getcwd() + "/" + executable_name
    subprocess.call(["gcc", "-lm", file_name, "-pthread"])

    sim_args = [(2,7),(2,9),(4,7),(4,9),(6,7),(6,9)]

    plot_file_name = "plot_data.csv"
    table_file_name = "table_data.csv"

    column_labels = ["(N,M)","Total Arrival", "Total Rejections", "Rejection Ratio",
                    "Average\nWait Time", "Max Waiting\nLine", "Max Waiting\nLine Time"]
    df_table = pd.DataFrame(columns = column_labels)

    arrivals_df_dict = {}
    waiting_df_dict = {}
    rejections_df_dict = {}

    for arg_tuple in sim_args:
        N = arg_tuple[0]
        M = arg_tuple[1]
        subprocess.call(["./a.out", str(N), str(M)])


        # If MAXPERCAR == 7, then save the plot data.
        if arg_tuple[1] == 7:
            df_plots = pd.read_csv(plot_file_name)

            arrivals_list = df_plots.ix[:,0]
            rejects_list = df_plots.ix[:,1]
            waiting_list = df_plots.ix[:,2]

            arrivals_df_dict[arg_tuple] = arrivals_list
            waiting_df_dict[arg_tuple] = waiting_list
            rejections_df_dict[arg_tuple] = rejects_list

        # Generate and save the table.
        table_data_file = open(table_file_name, "r")
        table_values = ((table_data_file.readline()).strip()).split(",")
        row_label = "N=" +  str(N) + ",M=" + str(M)

        # This is a bit sloppy, but I struggled with getting the rows updated
        # correctly without doing it explicitly
        new_table_values = [row_label, table_values[2], table_values[3], table_values[4], table_values[5], table_values[6], table_values[7]]
        df_table.loc[row_label] = new_table_values

    make_plots(arrivals_df_dict, waiting_df_dict, rejections_df_dict)
    make_table(df_table)


# make_plots()
def make_plots(arrivals_df_dict, waiting_df_dict, rejections_df_dict):
    x_axis = np.linspace(0, 600, 600)

    arrivals_fig = plt.figure()
    ax0 = arrivals_fig.add_subplot(111)
    for key, value in arrivals_df_dict.items():
        ax0.plot(x_axis, value, label=key)
    ax0.legend(loc=2)
    ax0.set_title("Arrivals Plot")
    ax0.set_xlabel("Time (minutes)")
    ax0.set_ylabel("Num. of Arrivals")
    ax0.grid(True)
    plt.savefig("arrivals-plot.png", bbox_inches='tight')

    waiting_fig = plt.figure()
    ax1 = waiting_fig.add_subplot(111)
    for key, value in waiting_df_dict.items():
        ax1.plot(x_axis, value, label=key)
    ax1.legend(loc=2)
    ax1.set_title("Waiting Line Plot")
    ax1.set_xlabel("Time (minutes)")
    ax1.set_ylabel("Num of People Waiting")
    ax1.grid(True)
    plt.savefig("waiting-plot.png", bbox_inches='tight')

    rejection_fig = plt.figure()
    ax2 = rejection_fig.add_subplot(111)
    for key, value in rejections_df_dict.items():
        ax2.plot(x_axis, value, label=key)
    ax2.legend(loc=2)
    ax2.set_title("Rejections Plot")
    ax2.set_xlabel("Time (minutes)")
    ax2.set_ylabel("Num of People Rejected")
    ax2.grid(True)
    plt.savefig("rejections-plot.png", bbox_inches='tight')


# make_table()
def make_table(df_table):
    # Making table of values
    fig, ax = plt.subplots()
    fig.patch.set_visible(False)
    ax.axis('off')

    # The bbox and tight_layout numbers had to be carefully found.
    table = ax.table(cellText = df_table.values, colLabels=df_table.columns, loc='center', bbox=[-0.38, -0.3, 1.4, 0.275]) # first value is left shift
    table.auto_set_font_size(False)
    table.set_fontsize(8)
    fig.tight_layout(rect=[0.2, 0.8, 1.02, 2.2]) # 3rd value is width of cells
    plt.savefig("table.png")

    plt.show()


if __name__ == "__main__":
    main()
