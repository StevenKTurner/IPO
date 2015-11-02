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
public class LuvToGray implements ColorspaceTransform{
    
    private BufferedImage LChannel;
    private BufferedImage uChannel;
    private BufferedImage vChannel;
    private WritableRaster Lr;
    private WritableRaster ur;
    private WritableRaster vr;
    //ref variables are White reference, based on https://en.wikipedia.org/wiki/Illuminant_D65 and maximum XYZColor class values for X, Y and Z
    private final static double Xref = .95047;
    private final static double Yref = 1.000;
    private final static double Zref = 1.08883;
    //epsilon and kappa from: http://www.brucelindbloom.com/Eqn_XYZ_to_Luv.html
    private final static double epsilon = 216.0/24389.0;
    private final static double kappa = 24389.0/27.0;
    //max and min values determined by testing and recording max/min values based on every sRGB color
    private final static double maxl = 100.00000386666655;
    private final static double minl = 0;
    private final static double maxu = 175.015029946927;
    private final static double minu = -83.07756224415779;
    private final static double maxv = 107.39854124004414;
    private final static double minv = -134.10294223604993;
    
    private static double upref;
    private static double vpref;
    
    //Testing variables:
//    static double lt;
//    static double ut;
//    static double vt;
    
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
        
        upref = (4 * Xref)/(Xref + (15*Yref) + (3*Zref));
        vpref = (9* Yref)/(Xref + (15*Yref)+(3*Zref));
    }

    public void setPixelColor(XYZColor xyz, int x, int y){
        double xval = xyz.getX();
        double yval = xyz.getY();
        double zval = xyz.getZ();
        
        double yref = (yval/Yref);
        
        double uprime;
        double vprime;
        if (!(xval == 0 && yval == 0 && zval == 0)){
            uprime = (4 * xval)/(xval + (15 * yval) + (3*zval));
            vprime = (9 * yval)/(xval + (15 * yval) + (3*zval));
        } else{
            uprime = 0;
            vprime = 0;
        }
        
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
        
        double lnormal = 255 * (l - minl)/(maxl - minl);
        double unormal = 255 * (u - minu)/(maxu - minu);
        double vnormal = 255 * (v - minv)/(maxv - minv);
        
        Lr.setSample(x, y, 0, (int) (lnormal + .5));
        ur.setSample(x, y, 0, (int) (unormal + .5));
        vr.setSample(x, y, 0, (int) (vnormal + .5));
        
        //variables for max/min testing
//        lt = l;
//        ut = u;
//        vt = v;
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
    
    public void writeGrayscaleImages(File location, String prefix) {
        try {
            ImageIO.write(LChannel, "PNG", new File(location, prefix + "LuvL.png"));
            ImageIO.write(uChannel, "PNG", new File(location, prefix + "Luvu.png"));
            ImageIO.write(vChannel, "PNG", new File(location, prefix + "Luvv.png"));
        } catch (IOException ex) {
            Logger.getLogger(LuvToGray.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Luv could not write images");
        }
    }
    
    public ColorSpace getColorSpace(){
        return ColorSpace.Luv;
    }
    
//    public static void main(String[] args) {
//        LuvToGray test = new LuvToGray(1,1);
//        double lmax = 0;
//        double lmin = 0;
//        double umax = 0;
//        double umin = 0;
//        double vmax = 0;
//        double vmin = 0;
//        for(int ri = 0; ri < 256; ri++){
//            for (int gi = 0; gi < 256; gi++){
//                for (int bi = 0; bi < 256; bi++){
//                    Color temp = TurnerUtil.invertGamma(new Color(ri, gi, bi));
//                    test.setPixelColor(temp, 0, 0);
//                    if (lmax < lt) lmax = lt;
//                    if (lmin > lt) lmin = lt;
//                    if (umax < ut) umax = ut;
//                    if (umin > ut) umin = ut;
//                    if (vmax < vt) vmax = vt;
//                    if (vmin > vt) vmin = vt;
//                }
//            }
//        }
//        System.out.println("Lmax = " + lmax);
//        System.out.println("Lmin = " + lmin);
//        System.out.println("Umax = " + umax);
//        System.out.println("Umin = " + umin);
//        System.out.println("Vmax = " + vmax);
//        System.out.println("Vmin = " + vmin);
//    }
    
}
