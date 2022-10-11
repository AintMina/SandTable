from matplotlib import pyplot as plt
import sys


# Creates a .png file from .thr file
def drawFile(file):
    # Craetes a base with almost no borders
    plt.figure(figsize=(20, 20), frameon=False)
    ax = plt.axes([0, 0, 1, 1], projection='polar')
    # Turns off grids and axis markings
    plt.axis('off')
    plt.grid(visible=None)

    # Inverting the y axis and turining the graph 90 degrees, because .thr does this
    ax.set_theta_direction(-1)
    ax.set_theta_zero_location("N")

    with open(file) as f:

        theta = []
        rho = []
        counter = 0
        for line in f:
            # discard if line commented
            if "#" in line or len(line) < 5 or "/" in line:
                continue
            
            else:
                comp = line.split()
                theta.append(float(comp[0]))
                rho.append(float(comp[1]))
                counter += 1


        plt.polar(theta, rho, marker='o', color='blue', markersize=1, linewidth=3)
        # Draws a green dot as starting point
        plt.polar(float(theta[0]), rho[0], marker='o', color='green', markersize=30)
        # Draws a red dot as end point
        plt.polar(float(theta[-1]), rho[-1], marker='o', color='red', markersize=30)

        name = file.split('.')
        plt.savefig(name[0]+'.png')

        return counter


if __name__ == "__main__":
    if len(sys.argv) > 1:
        name = sys.argv[1]
        drawFile(name)
    else:
        drawFile("media/tracks/sunburst.thr")
    

