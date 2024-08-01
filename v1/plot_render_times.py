import matplotlib.pyplot as plt
import numpy as np
import os
import matplotlib.cm as cm


def readfrominput(pathoffile):
    data = []
    with open(pathoffile, 'r') as file:
        for line in file:
            algorithm, height, width, duration = line.strip().split(',')
            data.append((algorithm, int(height), int(width), float(duration)))
    return data


def actyuallyplothtem(data):
    algorithms = sorted(set(item[0] for item in data))
    colors = cm.tab10(np.linspace(0, 1, len(algorithms)))
    dictoryohshitimeandirectory = 'renders'

    if not os.path.exists(dictoryohshitimeandirectory):
        os.makedirs(dictoryohshitimeandirectory)

    for algo, color in zip(algorithms, colors):
        plt.figure(figsize=(10, 6))

        filtered_data = [(w, d) for a, h, w, d in data if a == algo]
        if filtered_data:
            widths_filtered, durations = zip(*sorted(filtered_data))

            plt.scatter(widths_filtered, durations, color=color, s=30, label='Data points', edgecolor='black',
                        alpha=0.7)
            plt.plot(widths_filtered, durations, color=color, linestyle='-', linewidth=2, alpha=0.7)

            plt.xlabel('Width lol', fontsize=12)
            plt.ylabel('(seconds)', fontsize=12)
            plt.title(f'ALGO? {algo}', fontsize=14)
            plt.legend()
            plt.grid(True, linestyle='--', alpha=0.6)

            plt.tight_layout()
            plt.savefig(os.path.join(dictoryohshitimeandirectory, f'{algo}.png'))
            plt.close()

    # all on same graph
    plt.figure(figsize=(12, 8))

    for algo, color in zip(algorithms, colors):
        filtered_data = [(w, d) for a, h, w, d in data if a == algo]
        if filtered_data:
            widths_filtered, durations = zip(*sorted(filtered_data))
            plt.scatter(widths_filtered, durations, color=color, s=30, label=f'{algo} Data points', edgecolor='black',
                        alpha=0.7)
            plt.plot(widths_filtered, durations, color=color, linestyle='-', linewidth=2, alpha=0.7)

    plt.xlabel('adfhjdf', fontsize=12)
    plt.ylabel(' (seconds)', fontsize=12)
    plt.title('ALl algorithms', fontsize=16)
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.6)

    plt.tight_layout()
    plt.savefig(os.path.join(dictoryohshitimeandirectory, 'render_times_all_algorithms.png'))
    plt.close()


def main():
    data = readfrominput('render_times.txt')
    actyuallyplothtem(data)
    print("DONE")


if __name__ == "__main__":
    main()
