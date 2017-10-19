import bisect

#given a line defined by two points give the y-value for the given x-value
def interpolate_y(a=(1,1),b=(2,2),x=1.5):
   if b[0]==a[0]: return a[1]
   return ((float(b[1]-a[1])/float(b[0]-a[0]))*(x-a[0]))+a[1]

#This method will take a list of ordered pair and will integrate them by assuming that they are in order along the x-axis
def integrate(points=[]):
   points.sort()

   integral = 0
   for i in range(len(points)-1):
      trap = 0.5*(points[i+1][0]-points[i][0])*(points[i+1][1]+points[i][1])
      integral += trap


   return integral

def interpolate_x(a=(1,1),b=(2,2),y=1.5):
   if b[1] == a[1]: return a[0]
   return ((float(b[0]-a[0])/float(b[1]-a[1]))*(y-a[1]))+a[0]

#This will take a list of ordered pairs then reduce it to only consider the ones in x=0..(.5) and y=0..1
#it will integrate this an return the value
def intWindow(points=[]):
   points.sort()
   xlist,ylist = zip(*points)

   #Lets make a point at x=0.5
   a = bisect.bisect_left(xlist,0.5)
   b = bisect.bisect_right(xlist,0.5)

   #We need to handle several cases:

   if a!=b:
      #print "point with x=.5 exists in the list"
      pass
   elif a == len(xlist): #the points do not extend to 0.5
      y = interpolate_y(points[a-2],points[a-1],.5)
      points.append((0.5,y))
   elif a == 0:
      y = interpolate_y(points[a],points[a+1],.5)
      points.insert(a,(0.5,y))
   elif a==b:
      y = interpolate_y(points[a-1],points[a],.5)
      points.insert(a,(0.5,y))


   #Let's make a point at y=0
   a = bisect.bisect_left(ylist,0)
   b = bisect.bisect_right(ylist,0)   


   if a!=b:
      #print "point with y=0 exists in the list"
      pass
   elif a == len(ylist):
      x = interpolate_x(points[a-2],points[a-1],0)
      points.append((x,0))
   elif a == 0:
      x = interpolate_x(points[0],points[1],0)
      points.insert(0,(x,0))
   elif a==b:
      x = interpolate_x(points[a-1],points[a],0)
      points.insert(a,(x,0))

   points.sort()

   #now we need to get rid of points that are out of bounds.
   newSet = [(x,y) for (x,y) in points if x>=0 if x<=0.5 if y >=0]

   return integrate(newSet)


