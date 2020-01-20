import React from 'react';
import PropTypes from 'prop-types';
import { TextValidator, ValidatorForm, SelectValidator } from 'react-material-ui-form-validator';

import { withStyles } from '@material-ui/core/styles';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Button from '@material-ui/core/Button';
import Switch from '@material-ui/core/Switch';
import SaveIcon from '@material-ui/icons/Save';

import MenuItem from '@material-ui/core/MenuItem';
import isIP from '../validators/isIP';
import isHostname from '../validators/isHostname';
import or from '../validators/or';
import PasswordValidator from '../components/PasswordValidator';

const styles = theme => ({
  switchControl: {
    width: "100%",
    marginTop: theme.spacing(2),
    marginBottom: theme.spacing(0.5)
  },
  textField: {
    width: "100%"
  },
  selectField: {
    width: "100%",
    marginTop: theme.spacing(2),
    marginBottom: theme.spacing(0.5)
  },
  button: {
    marginRight: theme.spacing(2),
    marginTop: theme.spacing(2),
  }
});

class MQTTSettingsForm extends React.Component {

  componentWillMount() {
    ValidatorForm.addValidationRule('isIPOrHostname', or(isIP, isHostname));
  }

  render() {
    const { classes, mqttSettings, handleValueChange, handleCheckboxChange, onSubmit, onReset } = this.props;
    return (
      <ValidatorForm onSubmit={onSubmit}>
        <FormControlLabel className={classes.switchControl}
          control={
            <Switch
              checked={mqttSettings.enabled}
              onChange={handleCheckboxChange('enabled')}
              value="enabled"
              color="primary"
            />
          }
          label="Enable MQTT?"
        />
        <TextValidator
          validators={['required', 'isIPOrHostname']}
          errorMessages={['Host is required', "Not a valid IP address or hostname"]}
          name="host"
          label="Host"
          className={classes.textField}
          value={mqttSettings.host}
          onChange={handleValueChange('host')}
          margin="normal"
        />
        <TextValidator
          validators={['matchRegexp:^.{0,32}$']}
          errorMessages={['Base must be 32 characters or less']}
          name="base"
          label="Base"
          className={classes.textField}
          value={mqttSettings.base}
          onChange={handleValueChange('base')}
          margin="normal"
        />
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:1025', 'maxNumber:65535']}
          errorMessages={['Port is required', "Must be a number", "Must be greater than 1024 ", "Max value is 65535"]}
          name="port"
          label="Port"
          className={classes.textField}
          value={mqttSettings.port}
          type="number"
          onChange={handleValueChange('port')}
          margin="normal"
        />
        <TextValidator
          validators={['matchRegexp:^.{0,32}$']}
          errorMessages={['Username must be 32 characters or less']}
          name="username"
          label="Username"
          className={classes.textField}
          value={mqttSettings.username}
          onChange={handleValueChange('username')}
          margin="normal"
        />
        <PasswordValidator
          validators={['matchRegexp:^.{1,64}$']}
          errorMessages={['MQTT Point Password must be 64 characters or less']}
          name="password"
          label="Password"
          className={classes.textField}
          value={mqttSettings.password}
          onChange={handleValueChange('password')}
          margin="normal"
        />
        <SelectValidator name="qos" label="QOS" value={mqttSettings.qos} className={classes.selectField}
          onChange={handleValueChange('qos')}>
          <MenuItem value={0}>0 - At most once</MenuItem>
          <MenuItem value={1}>1 - At least once</MenuItem>
          <MenuItem value={2}>2 - Exactly once</MenuItem>
        </SelectValidator>
        <TextValidator
          validators={['required', 'isNumber', 'minNumber:60', 'maxNumber:86400']}
          errorMessages={['Value is required', 'Value must be a number', 'Must be at least 60 seconds', "Must not be more than 86400 seconds (24 hours)"]}
          name="keepalive"
          label="Keep Alive (Seconds)"
          className={classes.textField}
          value={mqttSettings.keepalive}
          type="number"
          onChange={handleValueChange('keepalive')}
          margin="normal"
        />
        <FormControlLabel className={classes.switchControl}
          control={
            <Switch
              checked={mqttSettings.retain}
              onChange={handleCheckboxChange('retain')}
              value="retain"
              color="primary"
            />
          }
          label="Retain"
        />
        <FormControlLabel className={classes.switchControl}
          control={
            <Switch
              checked={mqttSettings.will}
              onChange={handleCheckboxChange('will')}
              value="will"
              color="primary"
            />
          }
          label="Last will"
        />
        <FormControlLabel className={classes.switchControl}
          control={
            <Switch
              checked={mqttSettings.heartbeat}
              onChange={handleCheckboxChange('heartbeat')}
              value="heartbeat"
              color="primary"
            />
          }
          label="Heartbeat"
        />
        <Button startIcon={<SaveIcon />} variant="contained" color="primary" className={classes.button} type="submit">
          Save
        </Button>
        <Button variant="contained" color="secondary" className={classes.button} onClick={onReset}>
          Reset
        </Button>
      </ValidatorForm>
    );
  }
}

MQTTSettingsForm.propTypes = {
  classes: PropTypes.object.isRequired,
  mqttSettings: PropTypes.object,
  onSubmit: PropTypes.func.isRequired,
  onReset: PropTypes.func.isRequired,
  handleValueChange: PropTypes.func.isRequired,
};

export default withStyles(styles)(MQTTSettingsForm);
