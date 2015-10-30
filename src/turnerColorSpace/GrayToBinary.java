/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author s14003024
 */
public class GrayToBinary {
    
    BufferedImage[] images;
    BufferedImage[] binaryImages;
    
    public GrayToBinary(BufferedImage[] inputImages, Threshold threshold){
        images = inputImages;
        binaryImages = new BufferedImage[images.length];
        
        //the images and binaryImages arrays should have the same array entry (i.e. 0, 1, 2) pointing to images fromt the same source
        for (int i = 0; i < images.length; i++){
            binaryImages[i] = new BufferedImage(images[i].getWidth(), images[i].getHeight(), BufferedImage.TYPE_BYTE_BINARY);
            
            int threshNumber = threshold.getThresholdInt(images[i]);
            WritableRaster wr = binaryImages[i].getRaster();
            
            for (int h = 0; h < images[i].getHeight(); h++){
                for (int w = 0; w < images[i].getWidth(); w++){
                    if (images[i].getRaster().getSample(w, h, 0) > threshNumber){
                        wr.setSample(w, h, 0, 1);
                    } else {
                        wr.setSample(w, h, 0, 0);
                    }
                }
            }
        }
    }
    
    BufferedImage[] getBinaryImages(){
        return binaryImages;
    }
    
    public void writeBinaryImages(String prefix){
        try {
//            ImageIO.write(LChannel, "PNG", new File(prefix + "LabL.png"));
//            ImageIO.write(aChannel, "PNG", new File(prefix + "Laba.png"));
//            ImageIO.write(bChannel, "PNG", new File(prefix + "Labb.png"));
            for (int i = 0; i < binaryImages.length; i++){
                ImageIO.write(binaryImages[i], "PNG", new File(prefix + i + ".png"));
            }
        } catch (IOException ex) {
            Logger.getLogger(GrayToBinary.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Gray to Binary could not write images");
        }
    }
    
}
