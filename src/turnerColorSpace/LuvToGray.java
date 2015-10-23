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
import static java.lang.Double.isNaN;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/**
 *
 * @author s14003024
 */
public class LuvToGray implements ColorspaceToGray{
    
    private BufferedImage LChannel;
    private BufferedImage uChannel;
    private BufferedImage vChannel;
    private WritableRaster Lr;
    private WritableRaster ur;
    private WritableRaster vr;
    //Not sure if XN, YN and ZN are right.  Should be reference white for XYZ to Luv transformation, I see different values here: http://framewave.sourceforge.net/Manual/fw_function_020_0060_00330.html
    private final static double Xref = 95.0456;
    private final static double Yref = 100.0;
    private final static double Zref = 108.9058;
    private final static double epsilon = 216/24389;
    private final static double kappa = 24389/27;
    private final static double maxl = 100;
    private final static double maxu = -25.88282241911123; // this is wrong
    private final static double maxv = 3.1938623978394034; // this is wrong
    
    private BufferedImage[] LuvChannels = new BufferedImage[3];
    
    public LuvToGray(int width, int height){
        LChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        uChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        vChannel = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        
        LuvChannels[0] = LChannel;
        LuvChannels[1] = uChannel;
        LuvChannels[2] = vChannel;
        
        Lr = LChannel.getRaster();
        ur = uChannel.getRaster();
        vr = vChannel.getRaster();
    }

    public void setPixelColor(XYZColor xyz, int x, int y){
        double xval = xyz.getX();
        double yval = xyz.getY();
        double zval = xyz.getZ();
        
        double yref = (yval/Yref);

        double uprime = (4 * xval)/(xval + (15 * yval) + (3*zval));
        double vprime = (9 * yval)/(xval + (15 * yval) + (3*zval));
        double upref = (4 * Xref)/(Xref + (15*Yref) + (3*Zref));
        double vpref = (9* Yref)/(Xref + (15*Yref)+(3*Zref));
        
        double l;
        double u;
        double v;
        
        if (yref > epsilon){
            l = (116 * (Math.cbrt(yref))) - 16;
        } else {
            l = kappa * yref;
        }
        
        u = 13 * (l * (uprime - upref));
        v = 13 * (l * (vprime - vpref));
        
        if (isNaN(u)){
            u = 0.0;
        }
        if (isNaN(v)){
            v = 0.0;
        }
        
        double lnormal = l/maxl;
        double unormal = u/maxu;
        double vnormal = v/maxv;
        
        Lr.setSample(x, y, 0, (int) (lnormal*255));
        ur.setSample(x, y, 0, (int) (unormal*255));
        vr.setSample(x, y, 0, (int) (vnormal*255));
        
//        System.out.println(l);
//        System.out.println(u);
//        System.out.println(v);
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
        return LuvChannels;
    }

    @Override
    public void writeGrayscaleImages(String prefix) {
        try {
            ImageIO.write(LChannel, "PNG", new File(prefix + "LuvL.png"));
            ImageIO.write(uChannel, "PNG", new File(prefix + "Luvu.png"));
            ImageIO.write(vChannel, "PNG", new File(prefix + "Luvv.png"));
        } catch (IOException ex) {
            Logger.getLogger(LuvToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Luv could not write images");
        }
    }
    
//    public static void main(String[] args) {
//        LuvToGray test = new LuvToGray(1,1);
//        for(int blech = 0; blech < 256; blech++){
//            test.setPixelColor(0, blech, 0, 0, 0);
//        }
//    }
    
}
