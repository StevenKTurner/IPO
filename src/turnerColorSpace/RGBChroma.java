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
public class RGBChroma implements ColorspaceToGray{
    
    private BufferedImage r;
    private BufferedImage g;
    private BufferedImage b;
    private WritableRaster rr;
    private WritableRaster gr;
    private WritableRaster br;
    
    private BufferedImage[] rgbChromaChannels = new BufferedImage[3];
    
    public RGBChroma(int width, int height){
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
        float rPix;
        float gPix;
        float bPix;
        
        if ((R+G+B) != 0){
            rPix = R/(R+G+B);
            gPix = G/(R+G+B);
            bPix = B/(R+G+B);
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
            ImageIO.write(r, "PNG", new File(prefix + "Channelr.png"));
            ImageIO.write(g, "PNG", new File(prefix + "Channelg.png"));
            ImageIO.write(b, "PNG", new File(prefix + "Channelb.png"));
        } catch (IOException ex) {
            Logger.getLogger(OhtaToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("rgb Chroma could not write images");
        }
    }
    
}
