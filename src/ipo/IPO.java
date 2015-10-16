/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ipo;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author Steven
 */
public class IPO {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        
        BufferedImage rgbImage;
        BufferedImage greyscaleImage;
        
        try {
            rgbImage = ImageIO.read(new File("RGB16Million.png"));
            int height = rgbImage.getHeight();
            int width = rgbImage.getWidth();
            greyscaleImage = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
            WritableRaster raster = greyscaleImage.getRaster();
            
            for (int h=0; h<height; h++){
                for (int w=0; w<width; w++){
                    Color c = new Color(rgbImage.getRGB(w, h));
                    int R = c.getRed();
                    int G = c.getGreen();
                    int B = c.getBlue();
                    int l = (Math.max(Math.max(R, G), B) + Math.min(Math.min(R, G), B))/2;
                    raster.setSample(w, h, 0, l);
                }
            }
            
            ImageIO.write(greyscaleImage, "PNG", new File("greyscale.png"));
        } catch (IOException ex) {
            Logger.getLogger(IPO.class.getName()).log(Level.SEVERE, null, ex);
        }
        
    }
    
}
