# VIN Table



* TOC line
{:toc}

The VIN 'code' can be determined by looking at the first 6 digits of your vehicles VIN number.

```warning
For American vehicles, you need your vehicles FIN, not VIN. American markets seem to have 2 different values for these numbers. You can decode your VIN to FIN using [this site](https://www.lastvin.com/)
```

---
Example:

```
VIN: WDC203007A123456
```
This vehicle would be a `203` chassis, with `007` variant. This denotes what specifications your vehicle has, and what values must be set in the TCU. Therefore, when searching the tables, you can look for `203.007` in the variant column. Check the below tables for what values to store. 
---

```warning
A lot of chassis have crossover (Depending on year built, it might have a different CAN layer!)

This document might have a lot of duplicates. The duplicates will be removed over time
```

# EGS51 CAN Layer vehicles
{% include egs51.md %}

# EGS52 CAN Layer vehicles
{% include egs52.md %}

# EGS53 CAN Layer vehicles
TBA