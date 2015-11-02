/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;

/**
 *
 * @author Steven Turner
 */
public interface ColorspaceTransform {
    
    /**Takes R, G and B values and generates a grayscale pixel for different channels depending on the specific colorspace used.
     *
     * @param R Requires int R of red
     * @param G Requires int G of green
     * @param B Requires int B of blue
     */
    void setPixelColor(int R, int G, int B, int x, int y);
    /**Takes a Color and generates a grayscale pixel for different channels depending on the specific colorspace used.
     *
     * @param c Requires Color object from RGB image
     */
    void setPixelColor(Color c, int x, int y); 
    /**Takes an RGB int and generates a grayscale pixel for different channels depending on the specific colorspace used.
     * 
     * @param rgb 
     */
    void setPixelColor(int rgb, int x, int y);

    /**
     *
     * @return Returns an array of BufferedImages, each containing a grayscale version of a color channel
     */
    BufferedImage[] getGrayscaleImages();
    
    /**Writes the channels to PNG image files in the computer, starting with the given prefix
     *
     * @param prefix the prefix of the filename to be created
     */
    void writeGrayscaleImages(String prefix);
    void writeGrayscaleImages(File location, String prefix);
    
    ColorSpace getColorSpace();
}
