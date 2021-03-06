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
public class HSVToGray implements ColorspaceTransform{
    
    private BufferedImage SChannel;
    private BufferedImage VChannel;
    private WritableRaster Sr;
    private WritableRaster Vr;
    
    private BufferedImage[] SVChannels = new BufferedImage[2];
    
    public HSVToGray(int width, int height){
        SChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        VChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        SVChannels[0] = SChannel;
        SVChannels[1] = VChannel;
        
        Sr = SChannel.getRaster();
        Vr = VChannel.getRaster();
    }

    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        double S;
        double V;
        double Rcorrected = (R*1.0)/255;
        double Gcorrected = (G*1.0)/255;
        double Bcorrected = (B*1.0)/255;
        double max = Math.max(Bcorrected, Math.max(Rcorrected, Gcorrected));
        double min = Math.min(Bcorrected, Math.min(Rcorrected, Gcorrected));
        
        V = max;
        if ((max - min) == 0){
            S = 0;
        } else{
            S = (max-min)/V;
        }

        
        Sr.setSample(x, y, 0, S*255);
        Vr.setSample(x, y, 0, V*255);
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
        return SVChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(SChannel, "PNG", new File(prefix + "HSVSaturation.png"));
            ImageIO.write(VChannel, "PNG", new File(prefix + "HSVValue.png"));
        } catch (IOException ex) {
            Logger.getLogger(HSVToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("HSV could not write images");
        }
    }
    
    public void writeGrayscaleImages(File location, String prefix) {
        try {
            ImageIO.write(SChannel, "PNG", new File(location, prefix + "HSVSaturation.png"));
            ImageIO.write(VChannel, "PNG", new File(location, prefix + "HSVValue.png"));
        } catch (IOException ex) {
            Logger.getLogger(HSVToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("HSV could not write images");
        }
    }
    
    public ColorSpace getColorSpace(){
        return ColorSpace.HSV;
    }
    
}
