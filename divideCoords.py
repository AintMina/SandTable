import math


# Distance * length_of_the_arms to get units
# 0.03 seems a good distance
def checkDistance(coord1, coord2, distance):

    # Check if line is from center outwards
    if coord1[0] == coord2[0]:
        return False
    # Check if the line point are in the outer edge
    elif coord1[1] > 0.97 and coord2[1] > 0.97:
        return False

    # Changing polar coords to X and Y
    x1, y1 = polarToXY(coord1[0], coord1[1])
    x2, y2 = polarToXY(coord2[0], coord2[1])

    # Getting the change of coords
    deltaX = x2 - x1
    deltaY = y2 - y1

    # Getting the distance between the points
    d = math.sqrt(deltaX**2 + deltaY**2)

    # If distance is over twice the wanted distance
    if d > distance * 2:
        return True
    else:
        return False


# Change from polar to cartesian
def polarToXY(theta, r):
    x = r * math.cos(theta)
    y = r * math.sin(theta)

    return x, y


# Changes from cartesian to polar
def xyToPolar(x, y):
    r = round(math.sqrt(x**2 + y**2), 5)
    th = round(math.atan2(y, x), 5)

    return th, r


def divideBy(coord1, coord2, distance):

    # Changing polar coords to X and Y
    x1, y1 = polarToXY(coord1[0], coord1[1])
    x2, y2 = polarToXY(coord2[0], coord2[1])

    # Getting the change of coords
    deltaX = x2 - x1
    deltaY = y2 - y1

    # Getting the distance between the points
    d = math.sqrt(deltaX**2 + deltaY**2)

    # Calculating how many points it divides to
    num_points = int(d / distance)

    # If number of point is 0 then exit
    if num_points == 0:
        return False

    # Calculating the distance between points
    r = d / num_points

    # Getting the angle from point A to point B
    theta = math.atan2(deltaY, deltaX)

    # Starting the coord list with point A
    new_coords = [[x1, y1]]

    # Calculating all the points between
    for i in range(0, num_points):
        x_new = new_coords[-1][0] + r * math.cos(theta)
        y_new = new_coords[-1][1] + r * math.sin(theta)

        new_coords.append([x_new, y_new])

    # And adding the point B
    new_coords.append([x2, y2])

    coords_polar = []

    # Changing the points to polar
    for i in new_coords:
        th, r_new = xyToPolar(i[0], i[1])
        coords_polar.append([th, r_new])

        if th == coord2[0]:
            coords_polar.append(coord2)
            break

    return coords_polar


# Writing list to a file to test
def writeToFile(coords_polar):
    with open('test.thr', 'a') as f:
        for i in coords_polar:
            f.write(str(i[0]) + ' ' + str(i[1]) + '\n')


if __name__ == '__main__':
    coords = divideBy([math.pi /2, 1], [0, 1], 0.01)
    writeToFile(coords)