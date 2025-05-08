import { memo } from "react";
import { Sensor } from "../../../../../API/requests/measurementPointsRequests";
import { Badge, Col, Row } from "react-bootstrap";
import Tooltip from "../../../../components/Tooltip/Tooltip";

interface Props {
    sensor: Sensor,
}
const SensorItem = (props: Props) => {
    const { sensor } = props;

    return (
        <Row className="p-2 rounded-2 border border-1">
            <Col sm={4} lg={2} className="">
                <h5 className="p-0 m-0">
                    <Badge
                        bg={sensor.quantity === "temperature" ? "danger" : "warning"}
                        className="text-uppercase me-3">
                        <i className={`bi bi-${sensor.quantity === "temperature" ? "thermometer" : "arrows-move"}`} />
                        {sensor.quantity}
                    </Badge>
                </h5>
            </Col>
            <Col sm={8} lg={4} className="d-flex flex-row  align-items-center">
                <span>
                    <b> {sensor.name}</b>
                </span>
            </Col>
            <Col lg={6} className="d-flex flex-row gap-3 justify-content-end align-items-center">
                <Tooltip
                    tooltipText="Send Data Interval (seconds)"
                >
                    <Badge bg="primary">
                        <i className="bi bi-send me-2" />
                        {sensor.config.sendInterval} seconds
                    </Badge>
                </Tooltip>

                <Tooltip
                    tooltipText="Measure Data Interval (seconds)"
                >
                    <Badge bg="primary">
                        <i className="bi bi-activity me-2" />
                        {sensor.config.measureInterval} seconds
                    </Badge>
                </Tooltip>

                <Badge bg="danger" className="">
                    <i className="bi bi-thermometer-sun me-2" /> When temp below: {sensor.config.temperatureLimits.heating}  °C
                </Badge>

                <Badge bg="info" className="">
                    <i className="bi bi-snow me-2" />  When temp above: {sensor.config.temperatureLimits.cooling} °C
                </Badge>
            </Col>
        </Row>
    );

}

export default memo(SensorItem);