out_t[9] = bzero;

out_t[8] = bzero;

out_t[7] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[1]&~bit[0]) | (~bit[8]
    &~bit[7]&~bit[6]&~bit[4]&~bit[1]&~bit[0]) | (~bit[8]&~bit[7]&~bit[6]
    &~bit[3]&~bit[1]&~bit[0]);

out_t[6] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[2]&~bit[0]) | (~bit[8]
    &~bit[7]&~bit[6]&~bit[4]&~bit[2]&~bit[0]) | (~bit[8]&~bit[7]&~bit[6]
    &~bit[3]&~bit[2]&~bit[0]);

out_t[5] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]&~bit[0]) | (~bit[8]
    &~bit[7]&~bit[6]&~bit[4]&~bit[3]&~bit[0]);

out_t[4] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[0]) | (~bit[8]
    &~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&~bit[0]);

out_t[3] = (~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[0]) | (~bit[8]
    &~bit[7]&~bit[6]&bit[5]&~bit[3]&~bit[0]);

out_t[2] = bzero;

out_t[1] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[0]) | (~bit[8]&~bit[7]
    &~bit[6]&~bit[4]&~bit[0]) | (~bit[8]&~bit[7]&~bit[6]&~bit[3]&~bit[0]);

out_t[0] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[0]) | (~bit[8]&~bit[7]
    &~bit[6]&~bit[4]&~bit[0]) | (~bit[8]&~bit[7]&~bit[6]&~bit[3]&~bit[0]);

out[9]=(~disable_update&(out_t[9]))|(disable_update&out[9]);
out[8]=(~disable_update&(out_t[8]))|(disable_update&out[8]);
out[7]=(~disable_update&(out_t[7]))|(disable_update&out[7]);
out[6]=(~disable_update&(out_t[6]))|(disable_update&out[6]);
out[5]=(~disable_update&(out_t[5]))|(disable_update&out[5]);
out[4]=(~disable_update&(out_t[4]))|(disable_update&out[4]);
out[3]=(~disable_update&(out_t[3]))|(disable_update&out[3]);
out[2]=(~disable_update&(out_t[2]))|(disable_update&out[2]);
out[1]=(~disable_update&(out_t[1]))|(disable_update&out[1]);
out[0]=(~disable_update&(out_t[0]))|(disable_update&out[0]);
//control and disable_update update--
control=control&bit[0];
disable_update=disable_update|(~control);
//----------------1--------------
out_t[9] = bzero;

out_t[8] = (~bit[8]&~bit[7]&~bit[6]&~bit[3]&~bit[2]&~bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[2]&~bit[1]&bit[0]);

out_t[7] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&~bit[2]&~bit[1]
    &bit[0]) | (~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&~bit[2]
    &~bit[1]&bit[0]);

out_t[6] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[3]&~bit[1]
    &bit[0]) | (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]&bit[2]&~bit[1]
    &bit[0]) | (~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&~bit[1]
    &bit[0]) | (~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&~bit[2]
    &~bit[1]&bit[0]);

out_t[5] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&bit[2]&~bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[2]&~bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&bit[2]&~bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&~bit[3]&~bit[2]&~bit[1]&bit[0]);

out_t[4] = (~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[3]&~bit[1]&bit[0]);

out_t[3] = bzero;

out_t[2] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[1]&bit[0]) | (~bit[8]
    &~bit[7]&~bit[6]&~bit[4]&~bit[1]&bit[0]) | (~bit[8]&~bit[7]&~bit[6]
    &~bit[3]&~bit[1]&bit[0]);

out_t[1] = bzero;

out_t[0] = (~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[1]&bit[0]) | (
    ~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[1]&bit[0]) | (~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&~bit[3]&~bit[1]&bit[0]);

out[9]=(~disable_update&(out_t[9]))|(disable_update&out[9]);
out[8]=(~disable_update&(out_t[8]))|(disable_update&out[8]);
out[7]=(~disable_update&(out_t[7]))|(disable_update&out[7]);
out[6]=(~disable_update&(out_t[6]))|(disable_update&out[6]);
out[5]=(~disable_update&(out_t[5]))|(disable_update&out[5]);
out[4]=(~disable_update&(out_t[4]))|(disable_update&out[4]);
out[3]=(~disable_update&(out_t[3]))|(disable_update&out[3]);
out[2]=(~disable_update&(out_t[2]))|(disable_update&out[2]);
out[1]=(~disable_update&(out_t[1]))|(disable_update&out[1]);
out[0]=(~disable_update&(out_t[0]))|(disable_update&out[0]);
//control and disable_update update--
control=control&bit[1];
disable_update=disable_update|(~control);
//----------------2--------------
out_t[9] = bzero;

out_t[8] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]&~bit[2]&bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&~bit[2]&bit[1]&bit[0]);

out_t[7] = (~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&~bit[2]&bit[1]
    &bit[0]) | (~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]
    &bit[3]&~bit[2]&bit[1]&bit[0]);

out_t[6] = (bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[2]
    &bit[1]&bit[0]) | (bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]
    &~bit[2]&bit[1]&bit[0]) | (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]
    &~bit[3]&~bit[2]&bit[1]&bit[0]);

out_t[5] = (~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[2]&bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[3]&~bit[2]&bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&~bit[2]&bit[1]&bit[0]);

out_t[4] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[2]&bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[3]&~bit[2]&bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&~bit[2]&bit[1]&bit[0]) | (
    ~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&~bit[2]&bit[1]&bit[0]);

out_t[3] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[3]&~bit[2]
    &bit[1]&bit[0]) | (~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]
    &bit[4]&bit[3]&~bit[2]&bit[1]&bit[0]) | (~bit[10]&~bit[9]&~bit[8]
    &~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&~bit[2]&bit[1]&bit[0]);

out_t[2] = (~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[2]
    &bit[1]&bit[0]) | (bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[3]
    &~bit[2]&bit[1]&bit[0]) | (bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]
    &bit[3]&~bit[2]&bit[1]&bit[0]) | (bit[10]&~bit[8]&~bit[7]&~bit[6]
    &~bit[4]&bit[3]&~bit[2]&bit[1]&bit[0]) | (bit[9]&~bit[8]&~bit[7]
    &~bit[6]&~bit[4]&bit[3]&~bit[2]&bit[1]&bit[0]) | (~bit[10]&~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&~bit[2]&bit[1]&bit[0]);

out_t[1] = (~bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[3]&~bit[2]
    &bit[1]&bit[0]) | (~bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]
    &bit[3]&~bit[2]&bit[1]&bit[0]) | (~bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&bit[4]&~bit[3]&~bit[2]&bit[1]&bit[0]) | (~bit[9]&~bit[8]
    &~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[3]&~bit[2]&bit[1]&bit[0]) | (
    bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[3]&~bit[2]&bit[1]
    &bit[0]) | (~bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]
    &~bit[3]&~bit[2]&bit[1]&bit[0]) | (bit[10]&~bit[9]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&bit[3]&~bit[2]&bit[1]&bit[0]) | (bit[10]&~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&~bit[2]&bit[1]&bit[0]) | (
    bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[4]&~bit[3]&~bit[2]&bit[1]
    &bit[0]);

out_t[0] = (bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[3]&~bit[2]&bit[1]
    &bit[0]) | (bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&~bit[2]
    &bit[1]&bit[0]) | (~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]
    &~bit[2]&bit[1]&bit[0]) | (~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[4]
    &~bit[3]&~bit[2]&bit[1]&bit[0]) | (bit[10]&~bit[8]&~bit[7]&~bit[6]
    &bit[5]&bit[4]&~bit[3]&~bit[2]&bit[1]&bit[0]);

out[9]=(~disable_update&(out_t[9]))|(disable_update&out[9]);
out[8]=(~disable_update&(out_t[8]))|(disable_update&out[8]);
out[7]=(~disable_update&(out_t[7]))|(disable_update&out[7]);
out[6]=(~disable_update&(out_t[6]))|(disable_update&out[6]);
out[5]=(~disable_update&(out_t[5]))|(disable_update&out[5]);
out[4]=(~disable_update&(out_t[4]))|(disable_update&out[4]);
out[3]=(~disable_update&(out_t[3]))|(disable_update&out[3]);
out[2]=(~disable_update&(out_t[2]))|(disable_update&out[2]);
out[1]=(~disable_update&(out_t[1]))|(disable_update&out[1]);
out[0]=(~disable_update&(out_t[0]))|(disable_update&out[0]);
//control and disable_update update--
control=control&bit[2];
disable_update=disable_update|(~control);
//----------------3--------------
out_t[9] = (bit[11]&bit[10]&~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[12]&bit[9]&~bit[8]&~bit[7]&~bit[6]
    &bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]&bit[9]&~bit[8]
    &~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]);

out_t[8] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&~bit[3]&bit[2]&bit[1]
    &bit[0]) | (~bit[11]&~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]
    &~bit[3]&bit[2]&bit[1]&bit[0]);

out_t[7] = (bit[11]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[3]&bit[2]
    &bit[1]&bit[0]) | (bit[10]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]
    &~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[9]&~bit[8]&~bit[7]&~bit[6]
    &bit[5]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]);

out_t[6] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]);

out_t[5] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    ~bit[12]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]);

out_t[4] = (~bit[11]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]
    &bit[2]&bit[1]&bit[0]) | (bit[11]&~bit[8]&~bit[7]&~bit[6]&~bit[4]
    &~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[10]&~bit[8]&~bit[7]&~bit[6]
    &~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[9]&~bit[8]&~bit[7]
    &~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]&bit[10]
    &~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[3]&bit[2]&bit[1]&bit[0]);

out_t[3] = (~bit[12]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[3]&bit[2]
    &bit[1]&bit[0]) | (bit[11]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]
    &~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[10]&~bit[8]&~bit[7]&~bit[6]
    &bit[5]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[9]&~bit[8]
    &~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[4]&~bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]
    &~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]&~bit[10]&~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]);

out_t[2] = (bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[11]&~bit[10]
    &bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&~bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]
    &~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]&~bit[10]&bit[9]
    &~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&~bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[11]&~bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]&~bit[10]
    &bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]
    &bit[0]);

out_t[1] = (bit[11]&bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[11]&bit[10]&~bit[8]&~bit[7]&~bit[6]
    &~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[11]&~bit[10]&~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    ~bit[11]&~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&~bit[3]
    &bit[2]&bit[1]&bit[0]) | (bit[11]&~bit[10]&~bit[8]&~bit[7]&~bit[6]
    &bit[5]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]&~bit[11]
    &bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]
    &bit[0]);

out_t[0] = (bit[11]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&~bit[3]&bit[2]
    &bit[1]&bit[0]) | (bit[11]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]
    &~bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[11]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&~bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]&bit[9]
    &~bit[8]&~bit[7]&~bit[6]&bit[5]&bit[4]&~bit[3]&bit[2]&bit[1]&bit[0]) | (
    ~bit[12]&bit[11]&~bit[8]&~bit[7]&~bit[6]&bit[4]&~bit[3]&bit[2]&bit[1]
    &bit[0]) | (bit[11]&~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[4]
    &~bit[3]&bit[2]&bit[1]&bit[0]);

out[9]=(~disable_update&(out_t[9]))|(disable_update&out[9]);
out[8]=(~disable_update&(out_t[8]))|(disable_update&out[8]);
out[7]=(~disable_update&(out_t[7]))|(disable_update&out[7]);
out[6]=(~disable_update&(out_t[6]))|(disable_update&out[6]);
out[5]=(~disable_update&(out_t[5]))|(disable_update&out[5]);
out[4]=(~disable_update&(out_t[4]))|(disable_update&out[4]);
out[3]=(~disable_update&(out_t[3]))|(disable_update&out[3]);
out[2]=(~disable_update&(out_t[2]))|(disable_update&out[2]);
out[1]=(~disable_update&(out_t[1]))|(disable_update&out[1]);
out[0]=(~disable_update&(out_t[0]))|(disable_update&out[0]);
//control and disable_update update--
control=control&bit[3];
disable_update=disable_update|(~control);
//----------------4--------------
out_t[9] = bzero;

out_t[8] = (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]
    &bit[0]);

out_t[7] = bzero;

out_t[6] = (~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&bit[2]&bit[1]
    &bit[0]) | (~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]);

out_t[5] = (bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]&~bit[11]
    &~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[12]&~bit[11]&~bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]);

out_t[4] = (bit[10]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (bit[12]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]
    &~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]&~bit[9]&~bit[8]
    &~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    ~bit[12]&~bit[11]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]);

out_t[3] = (bit[10]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[12]&~bit[11]&bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[12]&bit[10]&~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (bit[12]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]
    &~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]&~bit[9]&~bit[8]
    &~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    ~bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]);

out_t[2] = (bit[12]&bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]&bit[10]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]
    &~bit[11]&bit[10]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]) | (bit[12]&~bit[10]&~bit[9]&~bit[8]&~bit[7]
    &~bit[6]&bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[11]
    &~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[12]&~bit[11]&~bit[10]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[10]
    &bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]
    &bit[0]);

out_t[1] = (bit[12]&~bit[11]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[12]&bit[11]&~bit[9]&~bit[8]&~bit[7]
    &~bit[6]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]&bit[11]
    &~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    ~bit[11]&bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]);

out_t[0] = (bit[12]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (bit[12]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&~bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]&bit[11]&bit[9]&~bit[8]
    &~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    ~bit[12]&bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&bit[5]&~bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]);

out[9]=(~disable_update&(out_t[9]))|(disable_update&out[9]);
out[8]=(~disable_update&(out_t[8]))|(disable_update&out[8]);
out[7]=(~disable_update&(out_t[7]))|(disable_update&out[7]);
out[6]=(~disable_update&(out_t[6]))|(disable_update&out[6]);
out[5]=(~disable_update&(out_t[5]))|(disable_update&out[5]);
out[4]=(~disable_update&(out_t[4]))|(disable_update&out[4]);
out[3]=(~disable_update&(out_t[3]))|(disable_update&out[3]);
out[2]=(~disable_update&(out_t[2]))|(disable_update&out[2]);
out[1]=(~disable_update&(out_t[1]))|(disable_update&out[1]);
out[0]=(~disable_update&(out_t[0]))|(disable_update&out[0]);
//control and disable_update update--
control=control&bit[4];
disable_update=disable_update|(~control);
//----------------5--------------
out_t[9] = bzero;

out_t[8] = (~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]);

out_t[7] = (bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]);

out_t[6] = (bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]);

out_t[5] = (bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[12]&~bit[10]&~bit[9]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[11]
    &~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]);

out_t[4] = (~bit[12]&bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[11]&bit[10]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[12]
    &bit[11]&~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]);

out_t[3] = (~bit[12]&~bit[11]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]) | (bit[12]&~bit[9]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[12]
    &bit[11]&bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]);

out_t[2] = (bit[11]&bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[11]&~bit[10]&bit[9]&~bit[8]
    &~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[11]&~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]
    &bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[13]&~bit[12]&~bit[11]&bit[10]
    &~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]
    &bit[0]) | (bit[12]&bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]
    &bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[12]&bit[11]&~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]);

out_t[1] = (~bit[12]&bit[11]&~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]
    &bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[12]&~bit[10]&~bit[9]
    &~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[12]&~bit[11]&bit[10]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]
    &bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (bit[13]&~bit[12]&~bit[11]
    &~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]
    &bit[0]) | (bit[12]&bit[11]&bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[13]&bit[12]
    &bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]
    &bit[1]&bit[0]) | (~bit[13]&bit[11]&~bit[10]&~bit[9]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]);

out_t[0] = (~bit[13]&bit[9]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]) | (~bit[13]&bit[11]&bit[10]&~bit[8]&~bit[7]
    &~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[13]
    &~bit[11]&~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]
    &bit[2]&bit[1]&bit[0]) | (bit[13]&~bit[11]&bit[10]&~bit[9]&~bit[8]
    &~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (
    bit[13]&bit[12]&bit[11]&~bit[10]&~bit[9]&~bit[8]&~bit[7]&~bit[6]
    &~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]&bit[0]) | (~bit[13]&~bit[12]
    &~bit[10]&~bit[8]&~bit[7]&~bit[6]&~bit[5]&bit[4]&bit[3]&bit[2]&bit[1]
    &bit[0]);

out[9]=(~disable_update&(out_t[9]))|(disable_update&out[9]);
out[8]=(~disable_update&(out_t[8]))|(disable_update&out[8]);
out[7]=(~disable_update&(out_t[7]))|(disable_update&out[7]);
out[6]=(~disable_update&(out_t[6]))|(disable_update&out[6]);
out[5]=(~disable_update&(out_t[5]))|(disable_update&out[5]);
out[4]=(~disable_update&(out_t[4]))|(disable_update&out[4]);
out[3]=(~disable_update&(out_t[3]))|(disable_update&out[3]);
out[2]=(~disable_update&(out_t[2]))|(disable_update&out[2]);
out[1]=(~disable_update&(out_t[1]))|(disable_update&out[1]);
out[0]=(~disable_update&(out_t[0]))|(disable_update&out[0]);
//control and disable_update update--
control=control&bit[5];
disable_update=disable_update|(~control);
//----------------6--------------
