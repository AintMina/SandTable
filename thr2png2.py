from matplotlib import pyplot as plt
import sys


def drawFile(file):
    plt.figure(figsize=(20, 20), frameon=False)
    ax = plt.axes([0, 0, 1, 1], projection='polar')
    plt.axis('off')
    plt.grid(visible=None)

    # Inverting the y axis and turining the graph 90 degrees
    ax.set_theta_direction(-1)
    ax.set_theta_zero_location("N")

    with open(file) as f:

        theta = []
        rho = []
        counter = 0
        for line in f:
            # discard if line commented
            if "#" in line or len(line) < 5:
                continue
            
            else:
                comp = line.split()
                theta.append(float(comp[0]))
                rho.append(float(comp[1]))
                counter += 1


        plt.polar(theta, rho, marker='o', color='blue', markersize=1, linewidth=3)
        plt.polar(float(theta[0]), rho[0], marker='o', color='green', markersize=30)
        plt.polar(float(theta[-1]), rho[-1], marker='o', color='red', markersize=30)

        name = file.split('.')
        plt.savefig(name[0]+'.png')

        return counter


if __name__ == "__main__":
    if len(sys.argv) > 1:
        name = sys.argv[1]
        drawFile(name)
    else:
        drawFile("media/tracks/wormhole.thr")
    

