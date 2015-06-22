int /* really double */
ldexp(x,e) /* return x * 2**e */
int x;	/* really double, but passed as float by pcc-20 */
int e;
{
    int ce,neg;
    unsigned cf,cs,mask;		/* MUST BE UNSIGNED */

    neg = (x < 0);
    cf = (x & 0777777777);		/* current fraction */
    ce = ((x >> 27) & 0377);		/* current biased exponent */
    mask = 1;				/* cannot use constants, sigh... */
    cs = ((unsigned)(x)) & (mask << 35);/* current sign */

    if (neg)
    {
        ce -= e;			/* new biased exponent */
	if (ce < 0)			/* overflow to -/+ infinity */
	    return (neg ? 0400000000001 : 0377777777777);
	else if (ce > 0377)		/* then underflow to zero */
	    return (0);
	else				/* new number */
            return( cs | (ce << 27) | cf );
    }
    else
    {
        ce += e;			/* new biased exponent */
	if (ce < 0)			/* then underflow to zero */
	    return (0);
	else if (ce > 0377)		/* overflow to -/+ infinity */
	    return (neg ? 0400000000001 : 0377777777777);
	else				/* new number */
            return( cs | (ce << 27) | cf );
    }

}
