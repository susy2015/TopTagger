#This method will take a list of ordered pair and will integrate them by assuming that they are in order along the x-axis
def integrate(points=[]):
   points.sort()

   integral = 0
   for i in range(len(points)-1):
      trap = 0.5*(points[i+1][0]-points[i][0])*(points[i+1][1]+points[i][1])
      integral += trap


   return integral
