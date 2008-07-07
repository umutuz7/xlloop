/*******************************************************************************
 * This program and the accompanying materials
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at 
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     Peter Smith
 *******************************************************************************/
package org.boris.xlloop.util;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.boris.variant.VTBoolean;
import org.boris.variant.VTCollection;
import org.boris.variant.VTDouble;
import org.boris.variant.VTFloat;
import org.boris.variant.VTInteger;
import org.boris.variant.VTLong;
import org.boris.variant.VTMap;
import org.boris.variant.VTNull;
import org.boris.variant.VTString;
import org.boris.variant.Variant;

/**
 * Used to map arguments to objects.
 */
public class VariantObjectConverter
{
    public static final Integer IZERO = new Integer(0);
    public static final Double DZERO = new Double(0);
    public static final Long LZERO = new Long(0);
    private ObjectRegistry registry = new ObjectRegistry();

    /**
     * Clear out the internal registry to release memory.
     */
    public void clearRegistry() {
        registry.clear();
        System.gc();
        Runtime.getRuntime().runFinalization();
    }

    /**
     * Get the size of the registry.
     * 
     * @return int.
     */
    public int getRegistrySize() {
        return registry.size();
    }

    /**
     * Creates an Variant from a java object (best attempt).
     * 
     * @param obj.
     * 
     * @return XLObject.
     */
    public Variant createFrom(Object obj) {
        if (obj instanceof String) {
            return new VTString((String) obj);
        } else if (obj instanceof Boolean) {
            return new VTBoolean(((Boolean) obj).booleanValue());
        } else if (obj instanceof Integer) {
            return new VTInteger(((Integer) obj).intValue());
        } else if (obj instanceof Float) {
            return new VTFloat(((Float) obj).floatValue());
        } else if (obj instanceof Double) {
            return new VTDouble(((Double) obj).doubleValue());
        } else if (obj instanceof String[]) {
            String[] arr = (String[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(arr[i]);
            }

            return array;
        } else if (obj instanceof String[][]) {
            String[][] arr = (String[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(arr[i][j]);
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof double[]) {
            double[] arr = (double[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(arr[i]);
            }

            return array;
        } else if (obj instanceof double[][]) {
            double[][] arr = (double[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(arr[i][j]);
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof Double[]) {
            Double[] arr = (Double[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(arr[i]);
            }

            return array;
        } else if (obj instanceof Double[][]) {
            Double[][] arr = (Double[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(arr[i][j]);
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof int[]) {
            int[] arr = (int[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(arr[i]);
            }

            return array;
        } else if (obj instanceof int[][]) {
            int[][] arr = (int[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(arr[i][j]);
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof Integer[]) {
            Integer[] arr = (Integer[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(arr[i]);
            }

            return array;
        } else if (obj instanceof Integer[][]) {
            Integer[][] arr = (Integer[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(arr[i][j]);
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof boolean[]) {
            boolean[] arr = (boolean[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(arr[i]);
            }

            return array;
        } else if (obj instanceof boolean[][]) {
            boolean[][] arr = (boolean[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(arr[i][j]);
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof Boolean[]) {
            Boolean[] arr = (Boolean[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(arr[i]);
            }

            return array;
        } else if (obj instanceof Boolean[][]) {
            Boolean[][] arr = (Boolean[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(arr[i][j]);
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof Object[][]) {
            Object[][] arr = (Object[][]) obj;
            VTCollection array = new VTCollection();

            for (int i = 0; i < arr.length; i++) {
                VTCollection r = new VTCollection();
                for (int j = 0; j < arr[0].length && j < arr[i].length; j++) {
                    r.add(createFrom(arr[i][j]));
                }
                array.add(r);
            }

            return array;
        } else if (obj instanceof Object[]) {
            Object[] arr = (Object[]) obj;
            VTCollection array = new VTCollection();
            for (int i = 0; i < arr.length; i++) {
                array.add(createFrom(arr[i]));
            }

            return array;
        } else if (obj instanceof Variant) {
            return (Variant) obj;
        } else if (obj != null) {
            return new VTString(registry.put(obj));
        } else {
            return VTNull.NULL;
        }
    }

    /**
     * Creates a java object from an XLObject.
     * 
     * @param obj.
     * @param hint.
     * 
     * @return Object.
     */
    public Object createFrom(Variant obj, Class hint) {
        if (obj instanceof VTString) {
            if (VTString.class.equals(hint)) {
                return obj;
            } else {
                String str = obj.toString();
                Object val = registry.get(str);
                if (val != null) {
                    return val;
                } else {
                    return str;
                }
            }
        } else if (obj instanceof VTLong) {
            if (Double.class.equals(hint) || double.class.equals(hint)) {
                return new Double(((VTLong) obj).longValue());
            } else if (String.class.equals(hint)) {
                return obj.toString();
            } else if (Integer.class.equals(hint) || int.class.equals(hint)) {
                return new Integer(((VTLong) obj).intValue());
            } else if (Boolean.class.equals(hint) || boolean.class.equals(hint)) {
                return new Boolean(((VTLong) obj).booleanValue());
            } else {
                return new Long(((VTLong) obj).longValue());
            }
        } else if (obj instanceof VTDouble) {
            if (String.class.equals(hint)) {
                return obj.toString();
            } else if (Integer.class.equals(hint) || int.class.equals(hint)) {
                return new Integer(((VTDouble) obj).intValue());
            } else if (Long.class.equals(hint) || long.class.equals(hint)) {
                return new Long(((VTDouble) obj).longValue());
            } else if (Boolean.class.equals(hint) || boolean.class.equals(hint)) {
                return new Boolean(((VTDouble) obj).booleanValue());
            } else {
                return new Double(((VTDouble) obj).doubleValue());
            }
        } else if (obj instanceof VTCollection) {
            return convertArray((VTCollection) obj, hint);
        } else if (obj instanceof VTMap) {
            VTMap st = (VTMap) obj;
            Map m = new HashMap();
            for (Iterator i = st.getKeys().iterator(); i.hasNext();) {
                String s = (String) i.next();
                m.put(s, createFrom(st.getValue(s), hint));
            }
            return m;
        } else if (obj instanceof VTNull || obj == null) {
            if (String.class.equals(hint)) {
                return "";
            } else if (int.class.equals(hint)) {
                return IZERO;
            } else if (long.class.equals(hint)) {
                return LZERO;
            } else if (boolean.class.equals(hint)) {
                return Boolean.FALSE;
            } else if (double.class.equals(hint)) {
                return DZERO;
            } else {
                return null;
            }
        }

        return null;
    }

    private Object convertVector(VTCollection arr, Class hint) {
        int length = arr.size();
        Object val = null;

        if (Integer.class.equals(hint)) {
            Long l = length > 0 ? arr.getLong(0) : null;
            return l == null ? null : new Integer(l.intValue());
        } else if (int.class.equals(hint)) {
            Long l = length > 0 ? arr.getLong(0) : null;
            return l == null ? IZERO : new Integer(l.intValue());
        } else if (Double.class.equals(hint)) {
            return length > 0 ? arr.getDouble(0) : null;
        } else if (double.class.equals(hint)) {
            Double d = length > 0 ? arr.getDouble(0) : null;
            return d == null ? DZERO : d;
        } else if (String.class.equals(hint)) {
            return length > 0 ? arr.getString(0) : null;
        } else if (double[].class.equals(hint)) {
            double[] darr = new double[length];

            for (int i = 0; i < darr.length; i++) {
                Double d = arr.getDouble(i);
                if (d != null) {
                    darr[i] = d.doubleValue();
                }
            }

            val = darr;
        } else if (double[][].class.equals(hint)) {
            double[][] darr = new double[length][1];

            for (int i = 0; i < length; i++) {
                Double d = arr.getDouble(i);
                if (d != null) {
                    darr[i][0] = d.doubleValue();
                }
            }

            val = darr;
        } else if (Double[].class.equals(hint)) {
            Double[] darr = new Double[length];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = arr.getDouble(i);
            }

            val = darr;
        } else if (Double[][].class.equals(hint)) {
            Double[][] darr = new Double[length][1];

            for (int i = 0; i < length; i++) {
                darr[i][0] = arr.getDouble(i);
            }

            val = darr;
        } else if (int[].class.equals(hint)) {
            int[] darr = new int[length];

            for (int i = 0; i < darr.length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i] = arr.getLong(i).intValue();
                }
            }

            val = darr;
        } else if (int[][].class.equals(hint)) {
            int[][] darr = new int[length][1];

            for (int i = 0; i < length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i][0] = l.intValue();
                }
            }

            val = darr;
        } else if (Integer[].class.equals(hint)) {
            Integer[] darr = new Integer[length];

            for (int i = 0; i < darr.length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i] = new Integer(l.intValue());
                }
            }

            val = darr;
        } else if (Integer[][].class.equals(hint)) {
            Integer[][] darr = new Integer[length][1];

            for (int i = 0; i < length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i][0] = new Integer(l.intValue());
                }
            }

            val = darr;
        } else if (boolean[].class.equals(hint)) {
            boolean[] darr = new boolean[length];

            for (int i = 0; i < darr.length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i] = l.intValue() == 1;
                }
            }

            val = darr;
        } else if (boolean[][].class.equals(hint)) {
            boolean[][] darr = new boolean[length][1];

            for (int i = 0; i < length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i][0] = l.intValue() == 1;
                }
            }

            val = darr;
        } else if (Boolean[].class.equals(hint)) {
            Boolean[] darr = new Boolean[length];

            for (int i = 0; i < darr.length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i] = new Boolean(l.intValue() == 1);
                }
            }

            val = darr;
        } else if (Boolean[][].class.equals(hint)) {
            Boolean[][] darr = new Boolean[length][1];

            for (int i = 0; i < length; i++) {
                Long l = arr.getLong(i);
                if (l != null) {
                    darr[i][0] = new Boolean(l.intValue() == 1);
                }
            }

            val = darr;
        } else if (String[].class.equals(hint)) {
            String[] darr = new String[length];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = arr.getString(i);
            }

            val = darr;
        } else if (String[][].class.equals(hint)) {
            String[][] darr = new String[length][1];

            for (int i = 0; i < length; i++) {
                darr[i][0] = arr.getString(i);
            }

            val = darr;
        } else if (Object[].class.equals(hint)) {
            Object[] darr = new Object[length];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = createFrom(arr.get(i), Object.class);
            }

            val = darr;
        } else if (Object[][].class.equals(hint)) {
            Object[][] darr = new Object[length][1];

            for (int i = 0; i < length; i++) {
                darr[i][0] = createFrom(arr.get(i), Object.class);
            }

            val = darr;
        } else {
            String str = arr.getCollection(0).getString(0);
            val = registry.get(str);
        }

        return val;
    }

    /**
     * Convert an array into the desired object.
     * 
     * @param arr.
     * @param hint.
     * 
     * @return Object.
     */
    private Object convertArray(VTCollection arr, Class hint) {
        Object val = null;
        VTCollection fr = arr.getCollection(0);
        if (fr == null) {
            return convertVector(arr, hint);
        }
        int width = fr.size();
        int height = arr.size();

        if (Integer.class.equals(hint) || int.class.equals(hint)) {
            val = new Integer(arr.getCollection(0).getLong(0).intValue());
        } else if (Double.class.equals(hint) || double.class.equals(hint)) {
            val = arr.getCollection(0).getDouble(0);
        } else if (String.class.equals(hint)) {
            val = arr.getCollection(0).getString(0);
        } else if (double[].class.equals(hint)) {
            double[] darr = new double[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = arr.getCollection(i / width).getDouble(i % width)
                        .doubleValue();
            }

            val = darr;
        } else if (double[][].class.equals(hint)) {
            double[][] darr = new double[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = arr.getCollection(i).getDouble(j)
                            .doubleValue();
                }
            }

            val = darr;
        } else if (Double[].class.equals(hint)) {
            Double[] darr = new Double[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = arr.getCollection(i / width).getDouble(i % width);
            }

            val = darr;
        } else if (Double[][].class.equals(hint)) {
            Double[][] darr = new Double[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = arr.getCollection(i).getDouble(j);
                }
            }

            val = darr;
        } else if (int[].class.equals(hint)) {
            int[] darr = new int[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = arr.getCollection(i / width).getLong(i % width)
                        .intValue();
            }

            val = darr;
        } else if (int[][].class.equals(hint)) {
            int[][] darr = new int[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = arr.getCollection(i).getLong(j).intValue();
                }
            }

            val = darr;
        } else if (Integer[].class.equals(hint)) {
            Integer[] darr = new Integer[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = new Integer(arr.getCollection(i / width).getLong(
                        i % width).intValue());
            }

            val = darr;
        } else if (Integer[][].class.equals(hint)) {
            Integer[][] darr = new Integer[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = new Integer(arr.getCollection(i).getLong(j)
                            .intValue());
                }
            }

            val = darr;
        } else if (boolean[].class.equals(hint)) {
            boolean[] darr = new boolean[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = arr.getCollection(i / width).getLong(i % width)
                        .intValue() == 1;
            }

            val = darr;
        } else if (boolean[][].class.equals(hint)) {
            boolean[][] darr = new boolean[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = arr.getCollection(i).getLong(j).intValue() == 1;
                }
            }

            val = darr;
        } else if (Boolean[].class.equals(hint)) {
            Boolean[] darr = new Boolean[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = new Boolean(arr.getCollection(i / width).getLong(
                        i % width).intValue() == 1);
            }

            val = darr;
        } else if (Boolean[][].class.equals(hint)) {
            Boolean[][] darr = new Boolean[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = new Boolean(arr.getCollection(i).getLong(j)
                            .intValue() == 1);
                }
            }

            val = darr;
        } else if (String[].class.equals(hint)) {
            String[] darr = new String[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = arr.getCollection(i / width).getString(i % width);
            }

            val = darr;
        } else if (String[][].class.equals(hint)) {
            String[][] darr = new String[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = arr.getCollection(i).getString(j);
                }
            }

            val = darr;
        } else if (Object[].class.equals(hint)) {
            Object[] darr = new Object[height * width];

            for (int i = 0; i < darr.length; i++) {
                darr[i] = createFrom(arr.getCollection(i / width)
                        .get(i % width), Object.class);
            }

            val = darr;
        } else if (Object[][].class.equals(hint)) {
            Object[][] darr = new Object[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    darr[i][j] = createFrom(arr.getCollection(i).get(j),
                            Object.class);
                }
            }

            val = darr;
        } else {
            String str = arr.getCollection(0).getString(0);
            val = registry.get(str);
        }

        return val;
    }

    public Object[] convert(VTCollection args, Class[] hints) {
        Object[] a = new Object[hints.length];
        int csize = args.size();
        for (int i = 0; i < hints.length; i++) {
            if (i < csize) {
                a[i] = createFrom(args.get(i), hints[i]);
            } else {
                a[i] = null;
            }
        }
        return a;
    }
}
