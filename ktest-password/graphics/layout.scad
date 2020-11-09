//linear_extrude(height = 5, center = true, convexity = 10)
//		import(file = "layout.dxf", layer = "plate");

linear_extrude(height = 2.5, center = true, convexity = 10)
import("layout.dxf", convexity=3);