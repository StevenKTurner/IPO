/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package turnerColorSpace;

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
 * @author s14003024
 */
public class HSLToGray implements ColorspaceTransform{
    
    private BufferedImage SChannel;
    private BufferedImage LChannel;
    private WritableRaster Sr;
    private WritableRaster Lr;
    
    private BufferedImage[] SLChannels = new BufferedImage[2];
    
    public HSLToGray(int width, int height){
        SChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        LChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        SLChannels[1] = SChannel;
        SLChannels[2] = LChannel;
        
        Sr = SChannel.getRaster();
        Lr = LChannel.getRaster();
    }

    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        double S;
        double L;
        double Rcorrected = (R*1.0)/255;
        double Gcorrected = (G*1.0)/255;
        double Bcorrected = (B*1.0)/255;
        double max = Math.max(Bcorrected, Math.max(Rcorrected, Gcorrected));
        double min = Math.min(Bcorrected, Math.min(Rcorrected, Gcorrected));
        
        L = (max + min)/2;
        
        if ((max - min) == 0){
            S = 0;
        } else {
            S = (max-min)/(1-Math.abs(2*L -1));
        }
        
        Sr.setSample(x, y, 0, S*255);
        Lr.setSample(x, y, 0, L*255);
    }

    @Override
    public void setPixelColor(Color c, int x, int y) {
        int R = c.getRed();
        int G = c.getGreen();
        int B = c.getBlue();
        
        setPixelColor(R, G, B, x, y);
    }

    @Override
    public void setPixelColor(int rgb, int x, int y) {
        Color c = new Color(rgb);
        setPixelColor(c, x, y);
    }

    @Override
    public BufferedImage[] getGrayscaleImages() {
        return SLChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(SChannel, "PNG", new File(prefix + "HSLSaturation.png"));
            ImageIO.write(LChannel, "PNG", new File(prefix + "HSLLightness.png"));
        } catch (IOException ex) {
            Logger.getLogger(HSLToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("HSL could not write images");
        }
    }
    
}
