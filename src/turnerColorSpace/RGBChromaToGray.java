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
public class RGBChromaToGray implements ColorspaceTransform{
    
    private BufferedImage r;
    private BufferedImage g;
    private BufferedImage b;
    private WritableRaster rr;
    private WritableRaster gr;
    private WritableRaster br;
    
    private BufferedImage[] rgbChromaChannels = new BufferedImage[3];
    
    public RGBChromaToGray(int width, int height){
        r = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        g = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        b = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        rgbChromaChannels[0] = r;
        rgbChromaChannels[1] = g;
        rgbChromaChannels[2] = b;
        
        rr = r.getRaster();
        gr = g.getRaster();
        br = b.getRaster();
    }

    @Override
    public void setPixelColor(int R, int G, int B, int x, int y) {
        double rPix;
        double gPix;
        double bPix;
        
        int sum = R+G+B;
        
        if ((R+G+B) != 0){
            rPix = (((R*1.0)/sum)*255);
            gPix = (((G*1.0)/sum)*255);
            bPix = (((B*1.0)/sum)*255);
        } else {
            rPix = 0;
            gPix = 0;
            bPix = 0;
        }
        
        rr.setSample(x, y, 0, rPix);
        gr.setSample(x, y, 0, gPix);
        br.setSample(x, y, 0, bPix);
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
        return rgbChromaChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(r, "PNG", new File(prefix + "rgChromaChannelr.png"));
            ImageIO.write(g, "PNG", new File(prefix + "rgChromaChannelg.png"));
            ImageIO.write(b, "PNG", new File(prefix + "rgChromaChannelb.png"));
        } catch (IOException ex) {
            Logger.getLogger(RGBChromaToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("rgb Chroma could not write images");
        }
    }
    
    public void writeGrayscaleImages(File location, String prefix) {
        try {
            ImageIO.write(r, "PNG", new File(location, prefix + "rgChromaChannelr.png"));
            ImageIO.write(g, "PNG", new File(location, prefix + "rgChromaChannelg.png"));
            ImageIO.write(b, "PNG", new File(location, prefix + "rgChromaChannelb.png"));
        } catch (IOException ex) {
            Logger.getLogger(RGBChromaToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("rgb Chroma could not write images");
        }
    }
    
    public ColorSpace getColorSpace(){
        return ColorSpace.rgbChroma;
    }
}
