import math


# 0.03 seems a good distance
def checkDistance(coord1, coord2, distance):
    if coord1[0] == coord2[0]:
        return False

    x1, y1 = polarToXY(coord1[0], coord1[1])
    x2, y2 = polarToXY(coord2[0], coord2[1])

    deltaX = x2 - x1
    deltaY = y2 - y1

    d = math.sqrt(deltaX**2 + deltaY**2)

    if d > distance * 2:
        return True
    else:
        return False


def polarToXY(theta, r):
    x = r * math.cos(theta)
    y = r * math.sin(theta)

    return x, y


def xyToPolar(x, y):
    r = round(math.sqrt(x**2 + y**2), 5)
    th = round(math.atan2(y, x), 5)

    return th, r


def divideBy(coord1, coord2, distance):
    x1, y1 = polarToXY(coord1[0], coord1[1])
    x2, y2 = polarToXY(coord2[0], coord2[1])

    deltaX = x2 - x1
    deltaY = y2 - y1

    d = math.sqrt(deltaX**2 + deltaY**2)

    num_points = int(d / distance)

    if num_points == 0:
        return 0

    r = d / num_points

    theta = math.atan2(deltaY, deltaX)

    new_coords = [[x1, y1]]

    for i in range(0, num_points):
        x_new = new_coords[-1][0] + r * math.cos(theta)
        y_new = new_coords[-1][1] + r * math.sin(theta)

        new_coords.append([x_new, y_new])

    new_coords.append([x2, y2])

    coords_polar = []

    for i in new_coords:
        th, r_new = xyToPolar(i[0], i[1])
        coords_polar.append([th, r_new])

        if th == coord2[0]:
            coords_polar.append(coord2)
            break

    return coords_polar


def writeToFile(coords_polar):
    with open('test.thr', 'a') as f:
        for i in coords_polar:
            f.write(str(i[0]) + ' ' + str(i[1]) + '\n')


if __name__ == '__main__':
    coords = divideBy([0, 0], [0, 1], 0.01)
    writeToFile(coords)