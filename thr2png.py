from matplotlib import pyplot as plt
import sys

plt.figure(figsize=(20, 20), frameon=False)
ax = plt.axes([0, 0, 1, 1], projection='polar')
plt.axis('off')
plt.grid(visible=None)


def drawFile(file):

    with open(file) as f:
        # background circle
        plt.polar(0, 0, markersize=1500, color='wheat', marker='o')

        theta = []
        rho = []
        delay = 1
        counter = 0
        for line in f:
            # discard if line commented
            if "#" in line or len(line) < 5:
                pass
            
            else:
                comp = line.split()
                theta.append(float(comp[0]))
                rho.append(float(comp[1]))
                if counter >= 1:
                    plt.polar([float(comp[0]), theta[counter-1]], [float(comp[1]), rho[counter-1]], marker='o', color='papayawhip', markersize=15, linewidth=15)
                else:
                    plt.polar(float(comp[0]), float(comp[1]), marker='o', color='papayawhip', markersize=15)
                
                if counter > delay:
                    if counter-delay >= 1:
                        t = [theta[counter-delay-1], theta[counter-delay]]
                        r = [rho[counter-delay-1], rho[counter-delay]]
                        plt.polar(t, r, marker='o', color='saddlebrown', markersize=10, linewidth=10)
                    else:
                        plt.polar(theta[counter-delay], rho[counter-delay], linewidth=10, marker='o', color='saddlebrown', markersize=10)
                counter += 1

        name = file.split('.')
        plt.savefig(name[0]+'.png')

        return counter


if __name__ == "__main__":
    if len(sys.argv) > 1:
        name = sys.argv[1]
        drawFile(name)
    else:
        drawFile("media/tracks/wormhole.thr")
    

