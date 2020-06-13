# 3D GRAPHICS ENGINE

3D graphic engine develop in Qt by ![Lucas Garcia](https://github.com/Skyway666) and ![Adrià Ferrer](https://github.com/Adria-F)

## Techniques implemented

### Mouse picking with textured identifiers

The mouse picking system is implemented by assigning a unique identifier to each object and rendering it to a texture.

<p>
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/objectIdentifiers.JPG" width="400"> 
</p>

Then retrieve the apropriate pixel value according to the mouse position on click, convert it back to a identifier and select the corresponding object.

### Selected object outline

The selected object can be identified by an outline. This outline can be configured to change thickness and color.

<p> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/selectionOutline1.JPG" width="400"> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/selectionOutline2.JPG" width="400"> 
</p>


### Background color and Infinite grid

The background color can be changed and will properly mix with the grid when active

<p>
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/infiniteGrid.JPG" width="400"> 
</p>

### Screen-Space Ambient Occlusion

A screen space ambient occlusion (SSAO) is calulated using normals and depth textures and then rendering itself into another texture.

<p>
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/ambientOcclusion.JPG" width="400"> 
</p>

This will be later used on the lighting pass as the ambient value.

<p> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/ambientOcclusionON.JPG" width="400"> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/ambientOcclusionOFF.JPG" width="400"> 
</p>
Ambient Occlusion ON/OFF correspondingly (without any other ilumination).

### Depth of field

A depth of field technique is implemented that allows to change the depth focus and blur only certain elements.

<p> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/DOF1.JPG" width="400"> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/DOF2.JPG" width="400"> 
</p>

### Local ilumination

<p>
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/localLighting.JPG" width="400"> 
</p>

### Storing geometry information

<p> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/position.JPG" width="200"> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/normals.JPG" width="200"> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/color.JPG" width="200"> 
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/depth.JPG" width="200"> 
</p>

### Configuring technique values

<p>
<img src="https://github.com/Adria-F/QT-Graphics-Engine/blob/master/Web%20Images/miscSettings.JPG" width="300"> 
</p>
