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
public class LMSToGray implements ColorspaceToGray{
    
    private BufferedImage LChannel;
    private BufferedImage MChannel;
    private BufferedImage SChannel;
    private WritableRaster Lr;
    private WritableRaster Mr;
    private WritableRaster Sr;
    
    private BufferedImage[] LMSChannels = new BufferedImage[3];
    
    public LMSToGray(int width, int height){
        LChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        MChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        SChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        LMSChannels[0] = LChannel;
        LMSChannels[1] = MChannel;
        LMSChannels[2] = SChannel;
        
        Lr = LChannel.getRaster();
        Mr = MChannel.getRaster();
        Sr = SChannel.getRaster();
    }

    public void setPixelColor(XYZColor xyz, int x, int y){
        double l = (.2897 * xyz.getX()) + (.689 * xyz.getY()) + (-0.0787 * xyz.getZ());
        double m = (-.2298 * xyz.getX()) + (1.1834 * xyz.getY()) + (0.0464 * xyz.getZ());
        double s = xyz.getZ();
        
        Lr.setSample(x, y, 0, (int) (l*255));
        Mr.setSample(x, y, 0, (int) (m*255));
        Sr.setSample(x, y, 0, (int) (s*255));
    }
    
    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        setPixelColor(new XYZColor(R, G, B), x, y);
    }

    @Override
    public void setPixelColor(Color c, int x, int y) {
        setPixelColor(new XYZColor(c), x, y);
    }

    @Override
    public void setPixelColor(int rgb, int x, int y) {
        Color c = new Color(rgb);
        setPixelColor(c, x, y);
    }

    @Override
    public BufferedImage[] getGrayscaleImages() {
        return LMSChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(LChannel, "PNG", new File(prefix + "Long.png"));
            ImageIO.write(MChannel, "PNG", new File(prefix + "Medium.png"));
            ImageIO.write(SChannel, "PNG", new File(prefix + "Short.png"));
        } catch (IOException ex) {
            Logger.getLogger(LMSToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("LMS could not write images");
        }
    }
    
}
