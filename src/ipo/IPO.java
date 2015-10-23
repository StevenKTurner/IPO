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
import turnerColorSpace.*;

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
        
        try {
            rgbImage = ImageIO.read(new File("Aal052.jpg"));
            int height = rgbImage.getHeight();
            int width = rgbImage.getWidth();
//            OhtaToGray otg = new OhtaToGray(width, height);
//            RGBChromaToGray rgbc = new RGBChromaToGray(width, height);
//            RGBToGray RGB = new RGBToGray(width, height);
//            HSLToGray hsl = new HSLToGray(width, height);
            LuvToGray test = new LuvToGray(width, height);
            
            for (int h=0; h<height; h++){
                for (int w=0; w<width; w++){
                    Color rgbPixel = new Color(rgbImage.getRGB(w, h));
                    Color gammaRemovedPixel = TurnerUtil.invertGamma(rgbPixel);
                    XYZColor xyzPixel = new XYZColor(gammaRemovedPixel);
//                    otg.setPixelColor(rgbImage.getRGB(w,h), w, h);
//                    rgbc.setPixelColor(rgbImage.getRGB(w,h), w, h);
//                    RGB.setPixelColor(rgbImage.getRGB(w, h), w, h);
//                    hsl.setPixelColor(rgbImage.getRGB(w, h), w, h);
                    test.setPixelColor(xyzPixel, w, h);
                }
            }

//            otg.writeGrayscaleImages("Ohta");
//            rgbc.writeGrayscaleImages("rgbChroma");
//            RGB.writeGrayscaleImages("RGB");
//            hsl.writeGrayscaleImages("HSL");
            test.writeGrayscaleImages("Aal052");
            
        } catch (IOException ex) {
            Logger.getLogger(IPO.class.getName()).log(Level.SEVERE, null, ex);
        }
        
    }
    
}
